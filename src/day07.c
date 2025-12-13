#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAX_HT_KEY_TYPE             char *
#define RAX_HT_VALUE_TYPE           uint16_t
#define RAX_HT_HASH(KEY)            (RAX_HT_HASH_FUN((KEY), strlen((KEY))))
#define RAX_HT_KEY_EQUAL(X, Y)      (strcmp((X), (Y)) == 0)
#define RAX_HT_DESTROY_KEY(KEY)     free(KEY)
#define RAX_HT_NAME htsu
#include "rax_ht.h"

#define RAX_DA_IMPLEMENTATION
#include "rax_da.h"

// TODO: move to rax_lib
#define TODO()                                                          \
    do {                                                                \
        fprintf(stderr, "%s:%d: Not yet implemented\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE);                                             \
    } while (0)

union day07_operand {
    char *identifier;
    uint16_t signal;
};

enum day07_op {
    OP_NONE = 0,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_LSHIFT,
    OP_RSHIFT,
};

struct day07_inst {
    enum day07_op op;
    bool input1_is_signal;
    union day07_operand input1;
    bool input2_is_signal;
    union day07_operand input2;
    char *output;
};

bool day07_pop_integer(strv *instruction, uint16_t *n)
{
    strv word = strv_peek_word(*instruction);
    long x = -1;
    if (!strv_parse_long(word, &x) || x < 0)
        return false;

    *n = (uint16_t)x;
    strv_pop_word(instruction);
    return true;
}

bool day07_pop_identifier(strv *instruction, char **id)
{
    strv word = strv_peek_word(*instruction);
    for (size_t i = 0; i < word.size; i++)
        if (!isalpha(word.str[i])) return false;

    *id = strv_to_cstr_owned(word);
    strv_pop_word(instruction);
    return true;
}

bool day07_pop_arrow(strv *instruction)
{
    strv word = strv_peek_word(*instruction);
    if (!strv_eq_cstr(word, "->"))
        return false;

    strv_pop_word(instruction);
    return true;
}

bool day07_pop_op(strv *instruction, enum day07_op *op)
{
    strv word = strv_peek_word(*instruction);
    if (strv_eq_cstr(word, "AND"))
        *op = OP_AND;
    else if (strv_eq_cstr(word, "OR"))
        *op = OP_OR;
    else if (strv_eq_cstr(word, "NOT"))
        *op = OP_NOT;
    else if (strv_eq_cstr(word, "LSHIFT"))
        *op = OP_LSHIFT;
    else if (strv_eq_cstr(word, "RSHIFT"))
        *op = OP_RSHIFT;
    else
        return false;

    strv_pop_word(instruction);
    return true;
}

/* #ifndef DEBUG */
/* #define DEBUG */
/* #endif */

bool day07_parse_instruction(strv instruction, struct day07_inst *inst)
{
#ifdef DEBUG
    printf("1. instruction: \""strv_fmt"\"\n", strv_arg(instruction));
#endif

    memset(inst, 0, sizeof(*inst));
    instruction = strv_trim(instruction);

    // Line can start with a number (signal), lowercase letter
    // (identifier), or uppercase letter (unary NOT instruction)
    if (isupper(instruction.str[0])) {
        // NOT instruction
        assert(day07_pop_op(&instruction, &inst->op));
        assert(inst->op == OP_NOT);

        // Expect identifier
        assert(day07_pop_identifier(&instruction, &inst->input1.identifier));

        // ARROW + IDENTIFIER
        assert(day07_pop_arrow(&instruction));
        assert(day07_pop_identifier(&instruction, &inst->output));
        return true;
    } if (isdigit(instruction.str[0])) {
        // SIGNAL
        assert(day07_pop_integer(&instruction, &inst->input1.signal));
        inst->input1_is_signal = true;

        // Check for ARROW
        if (day07_pop_arrow(&instruction)) {
            // Next has to be an IDENTIFIER.
            assert(day07_pop_identifier(&instruction, &inst->output));
            return true;
        }

        // OPERATOR
        assert(day07_pop_op(&instruction, &inst->op));

        // SIGNAL or IDENTIFIER
        if (isdigit(instruction.str[0])) {
            assert(day07_pop_integer(&instruction, &inst->input2.signal));
            inst->input2_is_signal = true;
        } else {
            assert(day07_pop_identifier(&instruction, &inst->input2.identifier));
        }

        // ARROW + IDENTIFIER
        assert(day07_pop_arrow(&instruction));
        assert(day07_pop_identifier(&instruction, &inst->output));
        return true;
    } else if (islower(instruction.str[0])) {
        // IDENTIFIER
        assert(day07_pop_identifier(&instruction, &inst->input1.identifier));

        // TODO: Could be xy -> yx
        // Need to handle arrow here...
        // Check for ARROW
        if (day07_pop_arrow(&instruction)) {
            // Next has to be an IDENTIFIER.
            assert(day07_pop_identifier(&instruction, &inst->output));
            return true;
        }

        // Operator
        assert(day07_pop_op(&instruction, &inst->op));

        // SIGNAL or IDENTIFIER
        if (isdigit(instruction.str[0])) {
            assert(day07_pop_integer(&instruction, &inst->input2.signal));
            inst->input2_is_signal = true;
        } else {
            assert(day07_pop_identifier(&instruction, &inst->input2.identifier));
        }

        // ARROW + IDENTIFIER
        assert(day07_pop_arrow(&instruction));
        assert(day07_pop_identifier(&instruction, &inst->output));
        return true;
    } else {
        fprintf(stderr, "Unexpected word at beginning of instruction: "strv_fmt"\n", strv_arg(instruction));
        return false;
    }
}

// Execute instruction
// What do we use to hold state?
// a map of string -> uint16_t ?
bool day07_execute_instruction(htsu *signals, const struct day07_inst *inst)
{
    // SIGNAL -> output
    if (inst->op == OP_NONE) {
        if (inst->input1_is_signal) {
            // Simply update the value in the map
            htsu_set(signals, strdup(inst->output), inst->input1.signal);
        } else {
            // Pull out value & set.
            uint16_t *input_val = htsu_get(signals, inst->input1.identifier);
            /* assert(input_val != NULL); */
            if (!input_val) return false;

            htsu_set(signals, strdup(inst->output), *input_val);
        }

        return true;
    }

    // NOT ID/SIGNAL -> output
    if (inst->op == OP_NOT) {
        if (inst->input1_is_signal) {
            // NOT the value and put it in the map
            htsu_set(signals, strdup(inst->output), ~inst->input1.signal);
        } else {
            // Pull out value & set.
            uint16_t *input_val = htsu_get(signals, inst->input1.identifier);

            // TODO: Apparently the first line of input is:
            // NOT dq -> dr
            // So, these instructions aren't in order?
            // "A gate provides no signal until all of its inputs have a signal"
            // So... 0?
            // So, here we just return?
            /* assert(input_val != NULL); */
            if (!input_val) return false;

            htsu_set(signals, strdup(inst->output), ~*input_val);
        }

        return true;
    }

    // Remaining operators are binary
    // Pull out the 2 input signals
    uint16_t input1, input2;
    if (inst->input1_is_signal) {
        input1 = inst->input1.signal;
    } else {
        uint16_t *input_val = htsu_get(signals, inst->input1.identifier);
        /* assert(input_val != NULL); */
        if (!input_val) return false;
        input1 = *input_val;
    }

    if (inst->input2_is_signal) {
        input2 = inst->input2.signal;
    } else {
        uint16_t *input_val = htsu_get(signals, inst->input2.identifier);
        /* assert(input_val != NULL); */
        if (!input_val) return false;
        input2 = *input_val;
    }

    uint16_t output = 0;
    switch (inst->op) {
    case OP_AND:
        output = input1 & input2;
        break;
    case OP_OR:
        output = input1 | input2;
        break;
    case OP_LSHIFT:
        output = input1 << input2;
        break;
    case OP_RSHIFT:
        output = input1 >> input2;
        break;
    default:
        assert(false && "Unreachable!");
    }

    // Update map
    htsu_set(signals, strdup(inst->output), output);
    return true;
}

void day07_dump_parsed_instruction(strv raw_inst, struct day07_inst *parsed_inst)
{
    printf("Instruction: "strv_fmt"\n\n", strv_arg(raw_inst));
    printf("struct day07 inst {\n");

    char *op_str = "";
    switch (parsed_inst->op) {
    case OP_AND:
        op_str = "AND";
        break;
    case OP_OR:
        op_str = "OR";
        break;
    case OP_NOT:
        op_str = "NOT";
        break;
    case OP_LSHIFT:
        op_str = "LSHIFT";
        break;
    case OP_RSHIFT:
        op_str = "RSHIFT";
        break;
    case OP_NONE:
        op_str = "NONE";
        break;
    }

    printf("\top: %s\n", op_str);

    if (parsed_inst->input1_is_signal) {
        printf("\tinput1: %d\n", parsed_inst->input1.signal);
    } else {
        printf("\tinput1: %s\n", parsed_inst->input1.identifier);
    }

    // Only has input2 with certain OP codes
    switch (parsed_inst->op) {
    case OP_NOT:
    case OP_NONE:
        printf("\tinput2: NONE\n");
        break;
    default:
        if (parsed_inst->input2_is_signal) {
            printf("\tinput2: %d\n", parsed_inst->input2.signal);
        } else {
            printf("\tinput2: %s\n", parsed_inst->input2.identifier);
        }

        break;
    }

    printf("\toutput: %s\n", parsed_inst->output);
    printf("}\n");
}

int day07_entries_cmp(const void *first, const void *second)
{
    const htsu_entry **e1 = (const htsu_entry **)first;
    const htsu_entry **e2 = (const htsu_entry **)second;
    return strcmp((*e1)->key, (*e2)->key);
}

void day07_free_inst(struct day07_inst *inst)
{
    free(inst->output);
    if (!inst->input1_is_signal)
        free(inst->input1.identifier);
    if (!inst->input2_is_signal)
        free(inst->input2.identifier);
}

int day07_run_instructions(const char *input)
{
    strv_it lines = {0};
    strv_lines(&lines, input);

    struct day07_inst *insts = NULL;
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
#ifdef DEBUG
        printf("line: \""strv_fmt"\"\n", strv_arg(lines.sv));
#endif

        struct day07_inst *inst = da_append_ptr(insts);
        assert(day07_parse_instruction(lines.sv, inst));

#ifdef DEBUG
        day07_dump_parsed_instruction(lines.sv, inst);
#endif
    }

    htsu signals = {0};
    while (da_size(insts) > 0) {
        // Iterate from the back to have fewer element shifts on removal
        for (ptrdiff_t i = da_sizei(insts) - 1; i >= 0; i--) {
            // Execute as many instructions as we can each time.
            if (day07_execute_instruction(&signals, &insts[i])) {
                // Need to free memory here (╥﹏╥)
                day07_free_inst(&insts[i]);
                da_drop(insts, i);
            }
        }
    }

    uint16_t *rp = htsu_get(&signals, "a");
    assert(rp && "a does not exist in the map!");
    int result = (int)*rp;

    htsu_destroy(&signals);
    da_free(insts);

    return result;
}

int day07_run_instructions_2(const char *input)
{
    strv_it lines = {0};
    strv_lines(&lines, input);

    struct day07_inst *insts = NULL;
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
#ifdef DEBUG
        printf("line: \""strv_fmt"\"\n", strv_arg(lines.sv));
#endif

        struct day07_inst *inst = da_append_ptr(insts);
        assert(day07_parse_instruction(lines.sv, inst));

#ifdef DEBUG
        day07_dump_parsed_instruction(lines.sv, inst);
#endif
    }

    // Need to make a clone of insts before we destroy it during
    // instruction execution
    struct day07_inst *insts_clone = NULL;
    da_reserve(insts_clone, da_size(insts) + 1);
    // Space should be reserved, just memcpy
    memcpy(insts_clone, insts, sizeof(*insts) * da_size(insts));
    rax__da_get_head(insts_clone)->size = da_size(insts);

    htsu signals = {0};
    while (da_size(insts) > 0) {
        // Iterate from the back to have fewer element shifts on removal
        for (ptrdiff_t i = da_sizei(insts) - 1; i >= 0; i--) {
            // Execute as many instructions as we can each time.
            if (day07_execute_instruction(&signals, &insts[i])) {
                // In this case, don't free the memory, because it is
                // still pointed to by the (shallow) clond
                // insts_clone.
                /* day07_free_inst(&insts[i]); */
                da_drop(insts, i);
            }
        }
    }

    uint16_t *a_ptr = htsu_get(&signals, "a");
    assert(a_ptr && "a does not exist in the map!");
    int a_value = (int)*a_ptr;

    // reset signal map
    htsu_clear(&signals);

    // set value of a to signal of b
    // Find the instruction that provides b's signal and change it
    for (size_t i = 0; i < da_size(insts_clone); i++) {
        if (strcmp(insts_clone[i].output, "b") == 0 && insts_clone[i].op == OP_NONE) {
            // ??? -> b
            insts_clone[i].input1.signal = (uint16_t)a_value;
        }
    }

    // Good 'ol copy/paste
    while (da_size(insts_clone) > 0) {
        // Iterate from the back to have fewer element shifts on removal
        for (ptrdiff_t i = da_sizei(insts_clone) - 1; i >= 0; i--) {
            if (day07_execute_instruction(&signals, &insts_clone[i])) {
                // Need to free memory here (╥﹏╥)
                day07_free_inst(&insts_clone[i]);
                da_drop(insts_clone, i);
            }
        }
    }

    a_ptr = htsu_get(&signals, "a");
    assert(a_ptr && "a does not exist in the map!");
    int result = (int)*a_ptr;

    htsu_destroy(&signals);
    da_free(insts);
    da_free(insts_clone);

    return result;
}

// This test function leaks memory.
// I'm too lazy and tired of this problem to care.
void day07_tests()
{
    const char *instructions = "123 -> x\n"
        "456 -> y\n"
        "x AND y -> d\n"
        "x OR y -> e\n"
        "x LSHIFT 2 -> f\n"
        "y RSHIFT 2 -> g\n"
        "NOT x -> h\n"
        "NOT y -> i\n";

    strv_it lines = {0};
    strv_lines(&lines, instructions);

    struct day07_inst *insts = NULL;
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
#ifdef DEBUG
        printf("line: \""strv_fmt"\"\n", strv_arg(lines.sv));
#endif

        struct day07_inst *inst = da_append_ptr(insts);
        assert(day07_parse_instruction(lines.sv, inst));
        day07_dump_parsed_instruction(lines.sv, inst);
    }

    // Seems like the instructions don't have to be executed in sequence.
    // Think we need to do multiple passes.
    // Pass 1: SIGNAL -> IDENTIFIER
    // Then, keep doing passes and update as we can.
    // Because a gate does not provide a signal until both inputs have a signal.
    //
    // x AND 32 -> a
    // 123 -> x
    //
    // So, on each pass, compute what we can, then remove the
    // instruction from the list of instructions.
    htsu signals = {0};
    while (da_size(insts) > 0) {
        // Iterate from the back to have fewer element shifts on removal
        for (ptrdiff_t i = da_sizei(insts) - 1; i >= 0; i--) {
            // Execute as many instructions as we can each time.
            if (day07_execute_instruction(&signals, &insts[i]))
                da_drop(insts, i); // leak ~~~🌊
        }
    }

    // Sorted results
    htsu_entry **entries = NULL;
    htsu_it it = htsu_iter(&signals);
    while (htsu_next(&it)) {
        htsu_entry *new = malloc(sizeof(*new));
        new->key = strdup(*it.key);
        new->value = *it.value;
        da_append(entries, new);
    }

    // qsort
    qsort(entries, da_size(entries), sizeof(*entries), day07_entries_cmp);

    for (int i = 0; i < da_sizei(entries); i++) {
        printf("%s: %u\n", entries[i]->key, entries[i]->value);
    }

    htsu_destroy(&signals);
    // free DA memory
    for (int i = 0; i < da_sizei(entries); i++)
        free(entries[i]);

    da_free(entries);
    da_free(insts);
}
