#ifndef SCPI_H_
#define SCPI_H_

#include <stddef.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define NUL '\0'
#define MAX_IN_STR_LEN 260
#define DEL_KEY 127
#define MAX_TOKEN_COUNT 16
#define MAX_ARG_LEN 16
#define MAX_TOKEN_LEN 80
#define ERROR_QUEUE_LENGTH 2
#define ERROR_MESSAGE_LEN 24

#define uint16_t int
#define uint8_t char
#define IO_pointers_t int

// #define MNEMONIC_TOO_LONG "-112,Program mnemonic too long"
// #define ARG_TOO_LONG "-112,Argument too long"
// #define BAD_PATH_OR_HEADER "-113,Bad path or header: "
// #define TOO_MANY_PARAMETERS "-108,Too many parameters: "
// #define TOO_FEW_PARAMETERS "-108,Too few parameters"

// const char *MNEMONIC_TOO_LONG   = "-112,Program mnemonic too long";
const char *ARG_TOO_LONG        = "-112,Argument too long: ";
const char *BAD_PATH_OR_HEADER  = "-113,Bad path or header: ";
const char *TOO_MANY_PARAMETERS = "-108,Too many parameters: ";
const char *TOO_FEW_PARAMETERS  = "-108,Too few parameters";
const char *NO_ERROR            = "+0,\"No error\"\r\n";

#define OPEN_SCPI_REGISTRY() uint16_t registry_index = 0
#define REGISTER_SCPI(cvar_name, scpi_node, parent, function, implied) static SCPI_Node_t cvar_name = {scpi_node, parent, function, implied}; scpi_nodes[registry_index++] = &cvar_name
#define CLOSE_SCPI_REGISTRY() scpi_nodes[registry_index] = NULL

typedef struct SCPI_Node_t
    {
    char scpi_node[MAX_TOKEN_LEN];                        // PGM_P scpi_node;
    const struct SCPI_Node_t *parent;
    void (*function)(char*, IO_pointers_t);
    const bool implied;
    } SCPI_Node_t;

typedef struct
    {
    char entered_value[MAX_TOKEN_LEN+1];
    const char *error_message;
    } SCPI_Error_t;

void setup_scpi_commands(SCPI_Node_t **scpi_nodes);
void process_scpi_input(SCPI_Node_t **scpi_nodes, IO_pointers_t IO);
void scpi_process_cmd_P( char* input_string, SCPI_Node_t **scpi_nodes, IO_pointers_t IO);
bool scpi_find_implied(SCPI_Node_t **current_command, char *token, SCPI_Node_t **scpi_nodes);
char *scpi_get_short_name(char *long_name);
void scpi_prStr_P(const char *arg, IO_pointers_t IO);
void sys_error_q(char *arg, IO_pointers_t IO);

int CDC_Device_ReceiveByte(void);
// void scpi_add_error_P(const char *error_message, IO_pointers_t IO);

void scpi_add_error_P(const char *error_message, char *bad_command, uint16_t max_length);
void st_OPC_q (char *arg, IO_pointers_t IO);
void scpi_IDN_q (char *arg, IO_pointers_t IO);
void scpi_get_range_q(char *arg, int IO);
void sys_rst_btloader(char *arg, int IO);

#endif