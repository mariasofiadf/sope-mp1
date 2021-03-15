#include "./mode_t_aux.h"

mode_t get_mask(enum user user, enum permission perm) {

    return 1 << ((perm) + (2-user)*3); 
}

int mode_contains(mode_t mode, enum user user, enum permission perm) { 
    return mode & get_mask(user, perm); 
}

mode_t mode_add(mode_t mode, enum user user, enum permission perm) {
    if(user != ALL)
        return mode | get_mask(user, perm);

    //Adding permissions for all users
    return mode |get_mask(GROUP, perm)| get_mask(OWNER, perm)|get_mask(OTHERS, perm);
}

mode_t mode_rm(mode_t mode, enum user user, enum permission perm) {
    if(user != ALL)
        return mode & ~get_mask(user, perm);
    //Removing permissions for all users
    return mode & ~(get_mask(GROUP, perm)| get_mask(OWNER, perm)|get_mask(OTHERS, perm));


}

mode_t change_perm(mode_t mode, enum user user, enum permission perm, enum symbol symb){
    switch (symb)
    {
    case ADD:
        return mode_add(mode, user, perm);
    case RM:
        return mode_rm(mode, user, perm);
    case SUBST:
        
        return mode_add(mode, user, perm);
    default:
        break;
    }
    return mode;
}
