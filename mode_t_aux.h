#ifndef MODE_T_AUX_H_
#define MODE_T_AUX_H_

#include <stdlib.h>
#include <stdio.h>

enum user { OWNER, GROUP, OTHERS, ALL};
enum symbol {RM, ADD, SUBST};
enum permission { EXECUTE, WRITE, READ};

struct modeInfo{
    enum user user;
    enum symbol symbol;
    u_int8_t read;
    u_int8_t write;
    u_int8_t execute;
};

mode_t get_mask(enum user user, enum permission perm);

int mode_contains(mode_t mode, enum user user, enum permission perm);

mode_t mode_add(mode_t mode, enum user user, enum permission perm);

mode_t mode_rm(mode_t mode, enum user user, enum permission perm) ;

mode_t change_perm(mode_t mode, enum user user, enum permission perm, enum symbol symb);




#endif  //MODE_T_AUX_H_
