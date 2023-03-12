# FLSH
### Flesh

![Element](https://i.imgur.com/NiUSy2t.gif)

**Description:**  *Flesh. Absorbs food and oxygen from BLOD and STMH, can be cooked.*

Flesh will die if burned by radiation particles (regardless of temperature), greater than 10 or less than -10 pressure, < -5 C or > 50 C, running out of oxygen or nutrients (small chance per frame), or touching "toxic" chemicals like H2O2, ACID, POLO, PLUT, URAN, ISOZ, ISZS, MERC and CAUS. After dying, it will slowly randomly decay into meat eating bacteria. The decay can be stopped by cooling flesh below 3 C or cooking it to above 40 C.

Alive flesh will slowly grow turn into skin (tmp4 = 1) around places touching NONE. Flesh will burn if too hot (around > 90 C). Flesh will evenly distribute nutrients to other flesh, udder and stomach. Flesh can gain oxygen from oxygenated blood (up to 50 per pixel per frame) and some nutrients (10 per pixel per frame). Oxygenated flesh is redder. Cooking to around 70 or 80 C makes the flesh change color. Frozen flesh is bluer in shade. Flesh is slightly flammable.

STKM can eat meat (>3 C, otherwise frozen and too hard) for 10 HP gain if its cooked, otherwise loses 5 HP.

SCP-009 can crystallize through flesh.


## Technical
#### Properties
**life:** Used for graphics, 1 = white part

**tmp:** Oxygen stored

**tmp2:** Nutrients stored

**tmp3:** Highest temperature

**tmp4:** 0 = Inside, 1 = Skin, 2 = Dead
