#include <stddef.h>

union day07_operand {
    char identifier;
    uint16_t signal;
};

enum day07_op {
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_SIGNAL,
};

struct day07_inst {
    enum day07_op op;
    bool input1_is_signal;
    union day07_operand input1;
    bool input2_is_signal;
    union day07_operand input2;
    char output;
};

bool day07_pop_integer(strv *instruction, uint16_t n)
{
    strv word == strv_next_word(*instruction);
    long x = -1;
    if (!strv_parse_long(word, &x) || x < 0)
        return false;

    *n = (uint16_t)x;
    strv_pop_word(instruction);
    return true;
}

bool day07_pop_identifier(strv *instruction, char *c)
{
    strv word = strv_next_word(*instruction);
    if (word.size != 1 || !isalpha(word.str[0]))
        return false;

    *c = word.str[0];
    strv_pop_word(instruction);
    return true;
}

bool day07_pop_arrow(strv *instruction)
{
    strv word = strv_next_word(*instruction);
    if (!strv_equal_cstr("->"))
        return false;

    strv_pop_word(instruction);
    return true;
}

bool day07_pop_op(strv *instruction, enum day07_op *op)
{
    strv word = strv_next_word(*instruction);
    if (strv_equal_cstr(word, "AND"))
        *op = OP_AND;
    else if (strv_equal_cstr(word, "OR"))
        *op = OP_OR;
    else if (strv_equal_cstr(word, "NOT"))
        *op = OP_NOT;
    else if (strv_equal_cstr(word, "LSHIFT"))
        *op = OP_LSHIFT;
    else if (strv_equal_cstr(word, "RSHIFT"))
        *op = OP_RSHIFT;
    else
        return false;

    strv_pop_word(instruction);
    return true;
}

bool day07_parse_instruction(strv instruction, struct day07_inst *inst)
{
#ifdef DEBUG
    printf("1. instruction: \""strv_fmt"\"\n", instruction);
#endif

    memset(inst, 0, sizeof(*inst));
    instruction = strv_trim(instruction);

    // Line can start with a number (signal), lowercase letter
    // (identifier), or uppercase letter (unary NOT instruction)
    if (isupper(instruction.str[0])) {
        // NOT instruction
        enum day07_op op;
        assert(day07_pop_op(&instruction, &op));
        assert(op == OP_NOT);

        // Expect identifier
        char id;
        assert(day07_pop_identifier(&instruction, &id));

        inst->op = op;
        inst->input1.identifier = id;
        return true;
    } if (isdigit(instruction.str[0])) {
        // SIGNAL
        uint16_t sig1;
        assert(day07_pop_integer(&instruction, &sig1));
        op->input1_is_signal = true;
        op->input1.signal = sig1;

        // Operator
        enum day07_op op;
        assert(day07_pop_op(&instruction, &op));
        op->op = op;

        // SIGNAL or IDENTIFIER
        if (isdigit(instruction.str[0])) {
            uint16_t sig2;
            assert(day07_pop_integer(&instruction, &sig2));
            op->input2_is_signal = true;
            op->input2.signal = sig2;
        } else {
            char id;
            assert(day07_pop_identifier(&instruction, &id));
            op->input2.identifier = id;
        }

        return true;
    } else if (islower(instruction.str[0])) {
        // IDENTIFIER
        char id1;
        assert(day07_pop_identifier(&instruction, &id1));
        op->input1.identifier = id1;

        // Operator
        enum day07_op op;
        assert(day07_pop_op(&instruction, &op));
        op->op = op;

        // SIGNAL or IDENTIFIER
        if (isdigit(instruction.str[0])) {
            uint16_t sig2;
            assert(day07_pop_integer(&instruction, &sig2));
            op->input2_is_signal = true;
            op->input2.signal = sig2;
        } else {
            char id2;
            assert(day07_pop_identifier(&instruction, &id2));
            op->input2.identifier = id2;
        }

        return true;
    } else {
        fprintf(stderr, "Unexpected word at beginning of instruction: "strv_fmt"\n", strv_arg(instruction));
        return false;
    }
}

// Execute instruction
// What do we use to hold state?
// a map of string -> uint16_t ?
bool day07_execute_instruction(const struct day07_inst *inst)
{

}

#if 0
bool day07_parse_instruction(strv instruction, struct day07_inst *inst)
{
#ifdef DEBUG
    printf("1. instruction: \""strv_fmt"\"\n", instruction);
#endif

    memset(inst, 0, sizeof(*inst));

    // Line can start with a number (signal), letter (identifier), or
    // unary instruction (only NOT)
    instruction = strv_trim(instruction);
    if (instruction[0] == 'N') {
        strv word = strv_pop_word(&instruction);
        if (!strv_equal_cstr(instruction, "NOT")) {
            fprintf(stderr, "Unexpected word at beginning of instruction: "strv_fmt"\n", strv_arg(word));
            return false;
        }

        // NOT instruction
        inst->op = OP_NOT;

        // There will be 1 operand
        word = strv_pop_word(&instruction);
        if (word.size != 1 || !isalpha(word.str[0])) {
            fprintf(stderr, "Unexpected an identifier, found: "strv_fmt"\n", strv_arg(word));
            return false;
        }

        inst->input1 = word.str[0];
        inst->input1_is_signal = false;

        // Pop ->
        word = strv_pop_word(&instruction);
        if (!strv_equal_cstr(instruction, "->")) {
            fprintf(stderr, "Expected to find '->', found: "strv_fmt"\n", strv_arg(word));
            return false;
        }

        // Will end in an identifier
        word = strv_pop_word(&instruction);
        if (word.size != 1 || !isalpha(word.str[0])) {
            fprintf(stderr, "Unexpected an identifier, found: "strv_fmt"\n", strv_arg(word));
            return false;
        }

        inst->output = word.str[0];
        return true;
    }

    // Continue...

    char *end = NULL;

    // NOTE: This is buggy, since it could parse beyond the 'end' of
    // the string view.
    long n = strtol(instruction.str, &end, 10);
    if (end == instruction.str) {
        // Could be NOT

        identifiers[0].let = instruction.str[0];
    } else {
        if (n < 0 || n > 65535) return false;
        identifiers[0].signal = (uint16_t)n;
    }

#ifdef TEST
    if (n > 0) {
        printf("2. Parsed signal: %u\n", identifiers[0].signal);
    } else {
        printf("2. Parsed letter: %c\n", identifiers[0].let);
    }
#endif




}
#endif


void day07_tests()
{

}
