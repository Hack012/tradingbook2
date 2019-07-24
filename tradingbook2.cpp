#include<eosio/eosio.hpp>
#include<eosio/asset.hpp>

using namespace eosio;

CONTRACT tradingbook2: public contract {
public:
using contract::contract;

[[eosio::on_notify("eosio.token::transfer")]]
void ontransfer(name from, name to, asset quantity, std::string memo) {
if(from == get_self()) {
ReceiveScope forSaveScope(get_self(), get_self().value);
auto itr = forSaveScope.find(to.value);
if(itr == forSaveScope.end()) {
forSaveScope.emplace(get_self(), [&](auto& row) {
row.myScope = to.value;
});
}

Receive forReceiver(get_self(), to.value);
forReceiver.emplace(get_self(), [&](auto& row) {
row.mykey = forReceiver.available_primary_key();
row.user = to;
row.balance = quantity;
});
} else {
SendScope forSaveScope(get_self(), get_self().value);
auto itr = forSaveScope.find(from.value);
if(itr == forSaveScope.end()) {
forSaveScope.emplace(get_self(), [&](auto& row) {
row.myScope = from.value;
});
}

Send forSender(get_self(), from.value);
forSender.emplace(get_self(), [&](auto& row) {
row.mykey = forSender.available_primary_key();
row.user = from;
row.balance = quantity;
});
}
}

ACTION eraseall() {
require_auth(get_self());

SendScope forSendScope(get_self(), get_self().value);
auto itrSend = forSendScope.begin();
while(itrSend != forSendScope.end()) {
Send forEraseAll(get_self(), itrSend->myScope);
auto itr = forEraseAll.begin();
while(itr != forEraseAll.end()) {
itr = forEraseAll.erase(itr);
}
itrSend = forSendScope.erase(itrSend);
}

ReceiveScope forReceiveScope(get_self(), get_self().value);
auto itrReceive = forReceiveScope.begin();
while(itrSend != forSendScope.end()) {
Receive forEraseAll(get_self(), itrReceive->myScope);
auto itr = forEraseAll.begin();
while(itr != forEraseAll.end()) {
itr = forEraseAll.erase(itr);
}
itrSend = forSendScope.erase(itrSend);
}
}

private:
TABLE tradingbook_struct {
uint64_t mykey;
name user;
asset balance;

uint64_t primary_key() const { return mykey; }
};

TABLE scopebook_struct {
uint64_t myScope;

uint64_t primary_key() const { return myScope; }
};

typedef multi_index<"sendbook"_n, tradingbook_struct> Send;
typedef multi_index<"receivebook"_n, tradingbook_struct> Receive;
typedef multi_index<"sendscope"_n, scopebook_struct> SendScope;
typedef multi_index<"receivescope"_n, scopebook_struct> ReceiveScope;
};