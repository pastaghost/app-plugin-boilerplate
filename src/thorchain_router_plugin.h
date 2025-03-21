#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
// EDIT THIS: Put in the number of selectors your plugin is going to support.
#define NUM_SELECTORS 1

// Name of the plugin.
// EDIT THIS: Replace with your plugin name.
#define PLUGIN_NAME "THORChain Router"

#define THORCHAIN_MEMO_MAX_LEN 250

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
// EDIT THIS: Change the naming (`selector_t`), and add your selector names.
typedef enum { DEPOSIT_WITH_EXPIRY = 0 } selector_t;

// Enumeration used to parse the smart contract data.
// EDIT THIS: Adapt the parameter names here.

typedef enum {
    VAULT = 0,    // Vault address
    ASSET,        // Assset contract address
    AMOUNT,       // Swap amount, in base decimal precision for token with address above
    MEMO_OFFSET,  // Memory offset to start of memo data (# bytes)
    MEMO_LENGTH,  // Number of characters in memo
    MEMO,         // THORChain memo string
    EXPIRATION,   // Expiration timestamp
    UNEXPECTED_PARAMETER
} parameter;

// EDIT THIS: Rename `BOILERPLATE` to be the same as the one initialized in `main.c`.
extern const uint32_t THORCHAIN_ROUTER_SELECTORS[NUM_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
// EDIT THIS: This struct is used by your plugin to save the parameters you parse. You
// will need to adapt this struct to your plugin.
typedef struct context_t {
    // For display.
    uint8_t vault[ADDRESS_LENGTH];
    uint8_t asset[ADDRESS_LENGTH];
    uint8_t amount[INT256_LENGTH];
    uint8_t memo_offset;  // Offset will always be 160 bytes, so uint8_t is fine here
    uint16_t memo_length;
    uint16_t memo_bytes_remaining;  // Remaining number of bytes to parse
    char memo[THORCHAIN_MEMO_MAX_LEN + 1];
    memo_t memo_data;  // Parsed memo data
    uint8_t memo_num_fields;
    uint8_t expiration[INT256_LENGTH];
    uint8_t token_found;

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

typedef struct memo_t {
    uint8_t asset[61];
    uint8_t dest_addr[41];
    uint8_t limit[10];      // Trade limit in 1e8 precision
    uint8_t affiliate[44];  // THORChain address length = 43, max THORName length = 30
    uint8_t fee[5];         // Fee, from 0-1000 basis points
} memo_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);

void parse_memo(context_t *context);
int hex_to_ascii(char *hex_str, char *ascii_str);