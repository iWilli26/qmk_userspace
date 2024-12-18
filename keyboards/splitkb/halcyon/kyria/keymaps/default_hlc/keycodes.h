#define EURO RALT(KC_5) //€
#define A_GRV RALT(KC_Z) //à
#define E_AIG RALT(KC_G) //é
#define E_GRV RALT(KC_F) //è
#define U_GRV RALT(KC_H) //ù
#define C_CED RALT(KC_C) //ç
#define D_CIR RALT(KC_6) //dead ^

#define LA_NAV LT(NAV, KC_SPC)
#define LA_SYM LT(SYM, KC_BSPC)
#define LA_FN LT(FN, KC_ENT)

#ifdef MAC
    #define HOME G(KC_LEFT)
    #define END G(KC_RGHT)

    #define REDO S(G(KC_Z))
    #define UNDO G(KC_Z)
    #define CUT G(KC_X)
    #define COPY G(KC_C)
    #define PASTE G(KC_V)
    #define SLCTALL G(KC_A)
    #define SAVE G(KC_S)

    #define PREV_W A(KC_LEFT)
    #define NEXT_W A(KC_RGHT)

    #define HOME_A LCTL_T(KC_A)
    #define HOME_S LGUI_T(KC_S)

    #define HOME_E LGUI_T(KC_E)
    #define HOME_O LCTL_T(KC_O)

    #define OS_CTL KC_LGUI
    #define OS_GUI KC_LCTL
#else
    #define HOME KC_HOME
    #define END KC_END

    #define REDO C(KC_Y)
    #define UNDO C(KC_Z)
    #define CUT C(KC_X)
    #define COPY C(KC_C)
    #define PASTE C(KC_V)
    #define SLCTALL C(KC_A)
    #define SAVE C(KC_S)

    #define PREV_W C(KC_LEFT)
    #define NEXT_W C(KC_RGHT)

    #define HOME_A LGUI_T(KC_A)
    #define HOME_S LCTL_T(KC_S)

    #define HOME_E LCTL_T(KC_E)
    #define HOME_O LGUI_T(KC_O)

    #define OS_CTL KC_LCTL
    #define OS_GUI KC_LGUI
#endif

#define HOME_R LALT_T(KC_R)
#define HOME_T LSFT_T(KC_T)

#define HOME_N LSFT_T(KC_N)
#define HOME_I LALT_T(KC_I)