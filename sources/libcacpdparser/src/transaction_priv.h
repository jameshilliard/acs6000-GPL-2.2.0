#ifndef TRANSACTION_PRIV_H_INCLUDED
#define TRANSACTION_PRIV_H_INCLUDED

#include "libcacpdparser/transaction.h"
#include "response_priv.h"

#include "commands_priv.h"

cacpdclient_transaction *create_transaction(enum command_type type);
void transaction_set_transmit_parameters(cacpdclient_transaction *trans, cacpdclient_connection_t *conn, int id);
void transaction_set_transmit_result_code(cacpdclient_transaction *trans, enum cacpdclient_transaction_result_code result );
cacpdclient_command_response *cacpdclient_get_response(cacpdclient_transaction *trans);

#endif /* TRANSACTION_PRIV_H_INCLUDED */
