/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

// Inline constexpr keymaps

// Bulgarian
{
    {
        {
            // cyrillic lowercase
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Minus, Qt::Key_Period },
            { Qt::Key_Comma, 0x0443 /*u*/, 0x0435 /*ie*/, 0x0438 /*i*/, 0x0448 /*sha*/, 0x0449 /*shcha*/, 0x043A /*ka*/, 0x0441 /*es*/, 0x0434 /*de*/, 0x0437 /*ze*/, 0x0446 /*tse*/ },
            { 0x044C /*soft sign*/, 0x044F /*ya*/, 0x0430 /*a*/, 0x043E /*o*/, 0x0436 /*zhe*/, 0x0433 /*ghe*/, 0x0442 /*te*/, 0x043D /*en*/, 0x0432 /*ve*/, 0x043C /*em*/, 0x0447 /*che*/ },
            { Qt::Key_CapsLock, 0x044E /*yu*/, 0x0439 /*short i*/, 0x044A /*hard sign*/, 0x044D /*e*/, 0x0444 /*ef*/, 0x0445 /*ha*/, 0x043F /*pe*/, 0x0440 /*er*/, 0x043B /*el*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_QuoteLeft, Qt::Key_ParenLeft, Qt::Key_Backslash, Qt::Key_Space, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            // cyrillic uppercase
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_Plus, Qt::Key_Question, Qt::Key_Percent, Qt::Key_Equal, Qt::Key_Colon, Qt::Key_Slash, Qt::Key_Underscore, 0x2116 /*no sign*/, 0x0406 /*byelorussian-ukrainian i*/, Qt::Key_V },
            { 0x044B /*small yeru*/, 0x0423 /*u*/, 0x0415 /*ie*/, 0x0418 /*i*/, 0x0428 /*sha*/, 0x0429 /*shcha*/, 0x041A /* ka*/, 0x0421 /*es*/, 0x0414 /*de*/, 0x0417 /*ze*/, 0x0426 /*tse*/ },
            { 0x042C /*soft sign*/, 0x042F /*ya*/, 0x0410 /*a*/, 0x041E /*o*/, 0x0416 /*zhe*/, 0x0413 /*ghe*/, 0x0422 /*te*/, 0x041D /*en*/, 0x0412 /*ve*/, 0x041C /*em*/, 0x0427 /*che*/ },
            { Qt::Key_Shift, 0x042E /* yu*/, 0x0419 /* short i*/, 0x042A /*hard sign*/, 0x042D /*e*/, 0x0424 /*ef*/, 0x0425 /*ha*/, 0x041F /*pe*/, 0x0420 /*er*/, 0x041B /*el*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiTilde, Qt::Key_ParenRight, Qt::Key_Semicolon, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_Slash },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde, Qt::Key_NumberSign },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Czech
{
    {
        {
            { Qt::Key_Plus, 0x011B /*e caron*/, 0x0161 /*s caron*/, 0x010D /*c caron*/, 0x0159 /*r caron*/, 0x017E /*z caron*/, LOWER( Qt::Key_Yacute ), LOWER( Qt::Key_Aacute ), LOWER( Qt::Key_Iacute ), LOWER( Qt::Key_Eacute ), Qt::Key_Equal, Qt::Key_Asterisk },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Uacute ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), 0x016F /*u ring*/, Qt::Key_section },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Semicolon, Qt::Key_ParenRight, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Percent, Qt::Key_Apostrophe },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Slash },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_QuoteDbl, Qt::Key_Exclam },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_degree, Qt::Key_ParenLeft, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Question, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_AsciiTilde, 0x011A /*E caron*/, 0x0160 /*S caron*/, 0x010C /*C caron*/, 0x0158 /*R caron*/, 0x017D /*Z caron*/, Qt::Key_Yacute, Qt::Key_Aacute, Qt::Key_Iacute, Qt::Key_Eacute, Qt::Key_currency },
            { Qt::Key_Backslash, Qt::Key_Bar, 0x20AC /*Euro*/, Qt::Key_unknown, Qt::Key_division, Qt::Key_multiply },
            { 0x0111 /*d stroke*/, 0x0110 /*D stroke*/, Qt::Key_BracketLeft, Qt::Key_BracketRight, 0x0142 /*l stroke*/, 0x0141 /*L stroke*/, Qt::Key_Dollar, Qt::Key_ssharp },
            { Qt::Key_NumberSign, Qt::Key_Ampersand, Qt::Key_At, Qt::Key_BraceLeft, Qt::Key_BraceRight, Qt::Key_Less, Qt::Key_Greater, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Asterisk, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// German
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_ssharp, Qt::Key_acute },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Udiaeresis ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Odiaeresis ), LOWER( Qt::Key_Adiaeresis ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiCircum, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_QuoteLeft },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Udiaeresis },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Odiaeresis, Qt::Key_Adiaeresis },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Apostrophe, Qt::Key_Asterisk, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar },
            { Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_NumberSign, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Danish
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Plus, Qt::Key_Apostrophe },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Aring ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_AE ), LOWER( Qt::Key_Ooblique ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_onehalf, Qt::Key_acute, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_currency, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Aring },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_AE, Qt::Key_Ooblique },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_section, Qt::Key_QuoteLeft, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_paragraph, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_copyright, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_onequarter, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_cent, Qt::Key_unknown, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_periodcentered, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Greek
{
    {
        {
            // lowercase
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Apostrophe, Qt::Key_Plus },
            { 0x00B7 /*middle dot*/, 0x03C2 /*sigma*/, 0x03B5 /*epsilon*/, 0x03C1 /*rho*/, 0x03C4 /*tau*/, 0x03C5 /*upsilon*/, 0x03B8 /*theta*/, 0x03B9 /*iota*/, 0x03BF /*omicron*/, 0x03C0 /*pi*/, 0x2019 /*right single quote*/ },
            { 0x03B1 /*alpha*/, 0x03C3 /*sigma*/, 0x03B4 /*delta*/, 0x03C6 /*phi*/, 0x03B3 /*gamma*/, 0x03B7 /*eta*/, 0x03BE /*xi*/, 0x03BA /*kappa*/, 0x03BB /*lamda*/, Qt::Key_BracketLeft, Qt::Key_BracketRight },
            { Qt::Key_CapsLock, 0x03B6 /*zeta*/, 0x03C7 /*chi*/, 0x03C8 /*psi*/, 0x03C9 /*omega*/, 0x03B2 /*beta*/, 0x03BD /*nu*/, 0x03BC /*mu*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_onehalf, Qt::Key_twosuperior, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            // uppercase
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_sterling, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_notsign, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_degree, Qt::Key_Asterisk },
            { 0x2015 /*horizontal bar*/, Qt::Key_brokenbar, 0x0395 /*epsilon*/, 0x03A1 /*rho*/, 0x03A4 /*tau*/, 0x03A5 /*upsilon*/, 0x0398 /*theta*/, 0x0399 /*iota*/, 0x039F /*omicron*/, 0x03A0 /*pi*/, 0x2018 /*left single quote*/ },
            { 0x0391 /*alpha*/, 0x03A3 /*sigma*/, 0x0394 /*delta*/, 0x03A6 /*phi*/, 0x0393 /*gamma*/, 0x0397 /*eta*/, 0x039E /*xi*/, 0x039A /*kappa*/, 0x039B /*lamda*/, Qt::Key_guillemotleft, Qt::Key_guillemotright },
            { Qt::Key_Shift, 0x0396 /*zeta*/, 0x03A7 /*chi*/, 0x03A8 /*psi*/, 0x03A9 /*omega*/, 0x0392 /*beta*/, 0x039D /*nu*/, 0x039C /*mu*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_plusminus, Qt::Key_threesuperior, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_paragraph, Qt::Key_sterling, 0x0384 /*tonos*/, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_copyright, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_onequarter, Qt::Key_threequarters, Qt::Key_Bar },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_cent, Qt::Key_unknown, Qt::Key_mu, },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_periodcentered, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// English (GB)
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_acute },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiCircum, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_QuoteLeft },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Apostrophe, Qt::Key_Asterisk, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar },
            { Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_NumberSign, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// English (US)
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Minus, Qt::Key_Equal },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), Qt::Key_Semicolon, Qt::Key_Apostrophe },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Comma, Qt::Key_Period, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_QuoteLeft, Qt::Key_Bar, Qt::Key_Space, Qt::Key_Slash, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_At, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_AsciiCircum, Qt::Key_Ampersand, Qt::Key_Asterisk, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Underscore, Qt::Key_Plus },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Colon, Qt::Key_QuoteDbl },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Less, Qt::Key_Greater, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiTilde, Qt::Key_Backslash, Qt::Key_Space, Qt::Key_Question, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight },
            { Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright },
            { },
            { Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Spanish
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Apostrophe, Qt::Key_exclamdown },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Ccedilla ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Ntilde ), Qt::Key_QuoteLeft },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_masculine, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_periodcentered, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_QuoteLeft },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Ccedilla },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Ntilde, Qt::Key_acute },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_ordfeminine, Qt::Key_Asterisk, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar },
            { Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde, Qt::Key_NumberSign },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Finnish
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Plus, Qt::Key_Apostrophe },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Aring ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Odiaeresis ), LOWER( Qt::Key_Adiaeresis ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_section, Qt::Key_acute, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_currency, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Aring },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Odiaeresis, Qt::Key_Adiaeresis },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_onehalf, Qt::Key_QuoteLeft, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_paragraph, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_copyright, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_onequarter, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_Bar },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_cent, Qt::Key_unknown, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_periodcentered, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// French
{
    {
        {
            { Qt::Key_Ampersand, LOWER( Qt::Key_Eacute ), Qt::Key_QuoteDbl, Qt::Key_Apostrophe, Qt::Key_ParenLeft, Qt::Key_Minus, LOWER( Qt::Key_Egrave ), Qt::Key_Underscore, LOWER( Qt::Key_Ccedilla ), LOWER( Qt::Key_Agrave ), Qt::Key_ParenRight, Qt::Key_Equal },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), Qt::Key_Asterisk },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_M ), LOWER( Qt::Key_Ugrave ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_W ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), Qt::Key_Comma, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_twosuperior, Qt::Key_Space, Qt::Key_Plus, Qt::Key_Exclam, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_degree, Qt::Key_mu },
            { Qt::Key_A, Qt::Key_Z, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_sterling },
            { Qt::Key_Q, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_M, Qt::Key_Ugrave },
            { Qt::Key_Shift, Qt::Key_W, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_Question, Qt::Key_Period, Qt::Key_Slash, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_threesuperior, Qt::Key_Space, Qt::Key_Percent, Qt::Key_section, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { 0x20AC /*Euro*/, Qt::Key_AsciiTilde, Qt::Key_NumberSign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_Bar, Qt::Key_QuoteLeft, Qt::Key_Backslash, Qt::Key_AsciiCircum, Qt::Key_At, Qt::Key_BracketRight, Qt::Key_BraceRight },
            { LOWER( Qt::Key_Agrave ), LOWER( Qt::Key_Egrave ), LOWER( Qt::Key_Igrave ), LOWER( Qt::Key_Ograve ), LOWER( Qt::Key_Ugrave ), Qt::Key_Agrave, Qt::Key_Egrave, Qt::Key_Igrave, Qt::Key_Ograve, Qt::Key_Ugrave, Qt::Key_unknown },
            { LOWER( Qt::Key_Acircumflex ), LOWER( Qt::Key_Ecircumflex ), LOWER( Qt::Key_Icircumflex ), LOWER( Qt::Key_Ocircumflex ), LOWER( Qt::Key_Ucircumflex ), Qt::Key_Acircumflex, Qt::Key_Ecircumflex, Qt::Key_Icircumflex, Qt::Key_Ocircumflex, Qt::Key_Ucircumflex },
            { LOWER( Qt::Key_Atilde ), LOWER( Qt::Key_Ntilde ), LOWER( Qt::Key_Otilde ), Qt::Key_Atilde, Qt::Key_Ntilde, Qt::Key_Otilde, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_unknown, Qt::Key_Space, Qt::Key_unknown, Qt::Key_unknown, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Hungarian
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, LOWER( Qt::Key_Odiaeresis ), LOWER( Qt::Key_Udiaeresis ), LOWER( Qt::Key_Oacute ) },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), 0x0151 /*o double acute*/ },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Eacute ), LOWER( Qt::Key_Aacute ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_0, LOWER( Qt::Key_Icircumflex ), LOWER( Qt::Key_Acircumflex ), Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Apostrophe, Qt::Key_QuoteDbl, Qt::Key_Plus, Qt::Key_Exclam, Qt::Key_Percent, Qt::Key_Slash, Qt::Key_Equal, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Odiaeresis, Qt::Key_Udiaeresis, Qt::Key_Oacute },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, 0x0150 /*O double acute*/ },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Eacute, Qt::Key_Aacute },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_section, Qt::Key_Icircumflex, Qt::Key_Acircumflex, Qt::Key_Space, Qt::Key_Question, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_currency },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_division, Qt::Key_multiply },
            { 0x0111 /*d stroke*/, 0x0110 /*D stroke*/, LOWER( Qt::Key_Adiaeresis ), Qt::Key_Adiaeresis, LOWER( Qt::Key_Iacute ), Qt::Key_Iacute, 0x0142 /*l stroke*/, 0x0141 /*L stroke*/, Qt::Key_Dollar, Qt::Key_ssharp },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_AsciiTilde, Qt::Key_Minus, Qt::Key_Underscore, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Italian
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Igrave, Qt::Key_Ugrave },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), Qt::Key_Egrave },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), Qt::Key_Ograve, Qt::Key_Agrave },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiCircum, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_QuoteLeft },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Eacute },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, LOWER( Qt::Key_Ccedilla ), Qt::Key_degree },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Apostrophe, Qt::Key_Asterisk, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar },
            { Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Japanese
{
    // Some special characters (backspace, enter) are replaced with Japanese equivalents for rendering purposes
    {
        {
            // katakana
            { 0x30A2 /*a*/, 0x30AB /*ka*/, 0x30B5 /*sa*/, 0x30BF /*ta*/, 0x30CA /*na*/, 0x30CF /*ha*/, 0x30DE /*ma*/, 0x30E4 /*ya*/, 0x30E9 /*ra*/, Qt::Key_Muhenkan /*backspace*/ },
            { 0x30A4 /*i*/, 0x30AD /*ki*/, 0x30B7 /*shi*/, 0x30C1 /*chi*/, 0x30CB /*ni*/, 0x30D2 /*hi*/, 0x30DF /*mi*/, Qt::Key_unknown, 0x30EA /*ri*/, Qt::Key_Mode_switch },
            { 0x30A6 /*u*/, 0x30AF /*ku*/, 0x30B9 /*su*/, 0x30C4 /*tsu*/, 0x30CC /*nu*/, 0x30D5 /*fu*/, 0x30E0 /*mu*/, 0x30E6 /*yu*/, 0x30EB /*ru*/, 0x3001 /*comma*/ },
            { 0x30A8 /*e*/, 0x30B1 /*ke*/, 0x30BB /*se*/, 0x30C6 /*te*/, 0x30CD /*ne*/, 0x30D8 /*he*/, 0x30E1 /*me*/, Qt::Key_unknown, 0x30EC /*re*/, Qt::Key_Kana_Lock },
            { 0x30AA /*o*/, 0x30B3 /*ko*/, 0x30BD /*so*/, 0x30C8 /*to*/, 0x30CE /*no*/, 0x30DB /*ho*/, 0x30E2 /*mo*/, 0x30E8 /*yo*/, 0x30ED /*ro*/, Qt::Key_Kanji /*enter*/ }
        }
    }, // v, n ???
    {
        {
            // katakana deck 2
            { 0x30A1 /*small a*/, 0x30AC /*ga*/, 0x30B6 /*za*/, 0x30C0 /*da*/, 0x30D0 /*ba*/, 0x30D1 /*pa*/, 0x30EF /*wa*/, 0x30E3 /*small ya*/, 0x30FB /*middle dot*/, Qt::Key_Muhenkan /*backspace*/ },
            { 0x30A3 /*small i*/, 0x30AE /*gi*/, 0x30B8 /*ji*/, 0x30C2 /*ji2*/, 0x30D3 /*bi*/, 0x30D4 /*pi*/, 0x30F0 /*wi*/, Qt::Key_unknown, 0x30FC /*prolonged sound mark*/, Qt::Key_Mode_switch },
            { 0x30A5 /*small u*/, 0x30B0 /*gu*/, 0x30BA /*zu*/, 0x30C5 /*zu2*/, 0x30D6 /*bu*/, 0x30D7 /*pu*/, 0x30F4 /*vu*/, 0x30E5 /*small yu*/, 0x30FD /*iteration mark*/, 0xFF61 /*period*/ },
            { 0x30A7 /*small e*/, 0x30B2 /*ge*/, 0x30BC /*ze*/, 0x30C7 /*de*/, 0x30D9 /*be*/, 0x30DA /*pe*/, 0x30F1 /*we*/, Qt::Key_unknown, 0x30FE /*voiced iteration mark*/, Qt::Key_Kana_Shift },
            { 0x30A9 /*small o*/, 0x30B4 /*go*/, 0x30BE /*zo*/, 0x30C9 /*do*/, 0x30DC /*bo*/, 0x30DD /*po*/, 0x30F2 /*wo*/, 0x30E7 /*small yo*/, 0x30F3 /*n*/, Qt::Key_Kanji /*enter*/ }
        }
    },
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Equal, Qt::Key_Muhenkan },
            { Qt::Key_At, Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_acute, Qt::Key_Mode_switch },
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiTilde },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_registered, Qt::Key_copyright, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_Question },
            { Qt::Key_Plus, Qt::Key_Asterisk, Qt::Key_Minus, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Kanji }
        }
    },
},

// Latvian
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Minus, LOWER( Qt::Key_F ) },
            { 0x016B /*u macron*/, LOWER( Qt::Key_G ), LOWER( Qt::Key_J ), LOWER( Qt::Key_R ), LOWER( Qt::Key_M ), LOWER( Qt::Key_V ), LOWER( Qt::Key_N ), LOWER( Qt::Key_Z ), 0x0113 /*e macron*/, 0x010D /*c caron*/, 0x017E /*z caron*/ },
            { 0x0161 /*s caron*/, LOWER( Qt::Key_U ), LOWER( Qt::Key_S ), LOWER( Qt::Key_I ), LOWER( Qt::Key_L ), LOWER( Qt::Key_D ), LOWER( Qt::Key_A ), LOWER( Qt::Key_T ), LOWER( Qt::Key_E ), LOWER( Qt::Key_C ), LOWER( Qt::Key_H ) },
            { Qt::Key_CapsLock, 0x0146 /*n cedilla*/, LOWER( Qt::Key_B ), 0x012B /*i macron*/, LOWER( Qt::Key_K ), LOWER( Qt::Key_P ), LOWER( Qt::Key_O ), 0x0101 /*a macron*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0123 /*g cedilla*/, 0x0137 /*k cedilla*/, 0x013C /*l cedilla*/, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Slash, Qt::Key_Ampersand, Qt::Key_multiply, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Underscore, Qt::Key_F },
            { 0x016A /*U macron*/, Qt::Key_G, Qt::Key_J, Qt::Key_R, Qt::Key_M, Qt::Key_V, Qt::Key_N, Qt::Key_Z, 0x0112 /*E macron*/, 0x010C /*C caron*/, 0x017D /*Z caron*/ },
            { 0x0160 /*S caron*/, Qt::Key_U, Qt::Key_S, Qt::Key_I, Qt::Key_L, Qt::Key_D, Qt::Key_A, Qt::Key_T, Qt::Key_E, Qt::Key_C, Qt::Key_H },
            { Qt::Key_Shift, 0x0145 /*N cedilla*/, Qt::Key_B, 0x012A /*I macron*/, Qt::Key_K, Qt::Key_P, Qt::Key_O, 0x0100 /*A macron*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0122 /*G cedilla*/, 0x0136 /*K cedilla*/, 0x013B /*L cedilla*/, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_acute },
            { Qt::Key_At, Qt::Key_Q, 0x0122 /*R cedilla*/, Qt::Key_W, Qt::Key_Y, Qt::Key_X, Qt::Key_Otilde, Qt::Key_Equal, 0x2013 /*en dash*/, Qt::Key_Question, Qt::Key_AsciiTilde },
            { Qt::Key_registered, LOWER( Qt::Key_Q ), 0x0157 /*r cedilla*/, LOWER( Qt::Key_W ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_Otilde ), 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_QuoteDbl, Qt::Key_NumberSign },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Lithuanian
{
    {
        {
            { 0x0105 /*a ogonek*/, 0x010D /*c caron*/, 0x0119 /*e ogonek*/, 0x0117 /*i ogonek*/, 0x0161 /*s caron*/, 0x0173 /*u ogonek*/, 0x016B /*u macron*/, Qt::Key_9, Qt::Key_0, Qt::Key_Minus, 0x017E /*z caron*/ },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), Qt::Key_Semicolon, Qt::Key_Apostrophe },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_QuoteLeft, Qt::Key_Backslash, Qt::Key_Slash, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { 0x0104 /*A ogonek*/, 0x010C /*C caron*/, 0x0118 /*E ogonek*/, 0x0116 /*I ogonek*/, 0x0160 /*S caron*/, 0x0172 /*U ogonek*/, 0x016A /*U macron*/, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Underscore, 0x017D /*Z caron*/ },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Colon, Qt::Key_QuoteDbl },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_AsciiTilde, Qt::Key_Bar, Qt::Key_Question, Qt::Key_Space, Qt::Key_Less, Qt::Key_Greater, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Equal },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_Percent, Qt::Key_Asterisk, Qt::Key_Plus, Qt::Key_Exclam },
            { Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceLeft, Qt::Key_BraceRight, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiCircum, Qt::Key_NumberSign },
            { Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Dutch
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Slash, Qt::Key_degree },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), Qt::Key_Asterisk },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), Qt::Key_Plus, Qt::Key_acute },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_At, Qt::Key_Less, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Underscore, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Apostrophe, Qt::Key_Question, Qt::Key_AsciiTilde },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Bar },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_plusminus, Qt::Key_QuoteLeft },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_section, Qt::Key_Greater, Qt::Key_Equal, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_paragraph, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_copyright, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar },
            { Qt::Key_ssharp, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright },
            { Qt::Key_cent, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_periodcentered, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

#if 0
// Norwegian
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Plus, Qt::Key_Apostrophe },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Aring ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Ooblique ), LOWER( Qt::Key_AE ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Bar, Qt::Key_Backslash, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_currency, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Aring },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Ooblique, Qt::Key_AE },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_section, Qt::Key_QuoteLeft, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_paragraph, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_copyright, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_cent, Qt::Key_unknown, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_periodcentered, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},
#endif

#if 0
// Polish
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Plus, Qt::Key_Apostrophe },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), 0x017C /*z dot*/ },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), 0x0142 /*l stroke*/, 0x0105 /*a ogonek*/ },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, LOWER( Qt::Key_Oacute ), 0x015B /*s acute*/, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_currency, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, 0x0144 /*n acute*/ },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, 0x0141 /*L stroke*/, 0x0119 /*E ogonek*/ },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x017A /*z acute*/, 0x0107 /*c acute*/, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_currency },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_division, Qt::Key_multiply },
            { Qt::Key_section, 0x0111 /*d stroke*/, 0x0110 /*D stroke*/, Qt::Key_AsciiTilde, Qt::Key_Dollar, Qt::Key_ssharp },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},
#endif

// Portuguese
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Apostrophe, Qt::Key_guillemotleft },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Plus ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Ccedilla ), Qt::Key_masculine },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Backslash, Qt::Key_acute, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_guillemotright },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Asterisk },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Ccedilla, Qt::Key_ordfeminine },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Bar, Qt::Key_QuoteLeft, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiCircum },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_AsciiTilde },
            { Qt::Key_unknown, Qt::Key_unknown, Qt::Key_unknown, Qt::Key_unknown, Qt::Key_unknown, Qt::Key_section },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Romanian
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Plus, Qt::Key_Apostrophe },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), 0x0103 /*a breve*/ },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), 0x015F /*s cedilla*/, 0x0163 /*t cedilla*/ },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_BracketRight, LOWER( Qt::Key_Icircumflex ), LOWER( Qt::Key_Acircumflex ), Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, 0x0102 /*A breve*/ },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, 0x015E /*S cedilla*/, 0x0162 /*T cedilla*/ },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_BracketLeft, Qt::Key_Icircumflex, Qt::Key_Acircumflex, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_currency },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_division, Qt::Key_multiply },
            { Qt::Key_section, 0x0111 /*d stroke*/, 0x0110 /*D stroke*/, 0x0142 /*l stroke*/, 0x0141 /*L stroke*/, Qt::Key_Dollar, Qt::Key_ssharp },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_AsciiTilde, Qt::Key_Minus, Qt::Key_Underscore, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Russian
{
    {
        {
            // cyrillic lowercase
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_Minus, Qt::Key_Equal },
            { 0x0439 /*short i*/, 0x0446 /*tse*/, 0x0443 /*u*/, 0x043A /*ka*/, 0x0435 /*ie*/, 0x043D /*en*/, 0x0433 /*ghe*/, 0x0448 /*sha*/, 0x0449 /*shcha*/, 0x0437 /*ze*/, 0x0445 /*ha*/ },
            { 0x0444 /*ef*/, 0x044B /*yeru*/, 0x0432 /*ve*/, 0x0430 /*a*/, 0x043F /*pe*/, 0x0440 /*er*/, 0x043E /*o*/, 0x043B /*el*/, 0x0434 /*de*/, 0x0436 /*zhe*/, 0x044D /*e*/ },
            { Qt::Key_CapsLock, 0x044F /*ya*/, 0x0447 /*che*/, 0x0441 /*es*/, 0x043C /*em*/, 0x0438 /*i*/, 0x0442 /*te*/, 0x044C /*soft sign*/, 0x0431 /*be*/, 0x044E /*yu*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0451 /*io*/, 0x044A /*hard sign*/, Qt::Key_Space, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            // cyrillic uppercase
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, 0x2116 /*no sign*/, Qt::Key_Semicolon, Qt::Key_Percent, Qt::Key_Colon, Qt::Key_Question, Qt::Key_Asterisk, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Underscore, Qt::Key_Plus },
            { 0x0419 /*short i*/, 0x0426 /*tse*/, 0x0423 /*u*/, 0x041A /*ka*/, 0x0415 /*ie*/, 0x041D /*en*/, 0x0413 /*ghe*/, 0x0428 /*sha*/, 0x0429 /*shcha*/, 0x0417 /*ze*/, 0x0425 /*ha*/ },
            { 0x0424 /*ef*/, 0x042B /*yeru*/, 0x0412 /*ve*/, 0x0410 /*a*/, 0x041F /*pe*/, 0x0420 /*er*/, 0x041E /*o*/, 0x041B /*el*/, 0x0414 /*de*/, 0x0416 /*zhe*/, 0x042D /*e*/ },
            { Qt::Key_Shift, 0x042F /*ya*/, 0x0427 /*che*/, 0x0421 /*es*/, 0x041C /*em*/, 0x0418 /*i*/, 0x0422 /*te*/, 0x042C /*soft sign*/, 0x0411 /*be*/, 0x042E /*yu*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0401 /*io*/, 0x042A /*hard sign*/, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_Slash },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_AsciiTilde, Qt::Key_NumberSign },
            { },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Slovene
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Apostrophe, Qt::Key_Plus },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), 0x0161 /*s caron*/ },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), 0x010D /*c caron*/, 0x0107 /*c acute*/ },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0111 /*d stroke*/, 0x017E /*z caron*/, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, 0x0160 /*S caron*/ },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, 0x010C /*C caron*/, 0x0106 /*C acute*/ },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, 0x0110 /*D stroke*/, 0x017D /*Z caron*/, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_currency },
            { Qt::Key_At, Qt::Key_registered, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_division, Qt::Key_multiply },
            { Qt::Key_section, Qt::Key_AsciiTilde,  0x0142 /*l stroke*/, 0x0141 /*L stroke*/, Qt::Key_ssharp },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Slovak
{
    {
        {
            { Qt::Key_Plus, 0x013E /*l caron*/, 0x0161 /*s caron*/, 0x010D /*c caron*/, 0x0165 /*t caron*/, 0x017E /*z caron*/, LOWER( Qt::Key_Yacute ), LOWER( Qt::Key_Aacute ), LOWER( Qt::Key_Iacute ), LOWER( Qt::Key_Eacute ), Qt::Key_Equal, 0x0148 /*n caron*/ },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Z ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), LOWER( Qt::Key_Uacute ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), LOWER( Qt::Key_Ocircumflex ), Qt::Key_section },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Y ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Semicolon, LOWER( Qt::Key_Adiaeresis ), Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Percent, Qt::Key_Asterisk },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_Slash },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_QuoteDbl, Qt::Key_Exclam },
            { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_degree, Qt::Key_Ampersand, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Question, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_AsciiTilde, 0x011A /*E caron*/, 0x0160 /*S caron*/, 0x010C /*C caron*/, 0x0158 /*R caron*/, 0x017D /*Z caron*/, Qt::Key_Yacute, Qt::Key_Aacute, Qt::Key_Iacute, Qt::Key_Eacute, Qt::Key_currency },
            { Qt::Key_Backslash, Qt::Key_Bar, 0x20AC /*Euro*/, Qt::Key_Apostrophe, Qt::Key_division, Qt::Key_multiply },
            { 0x0111 /*d stroke*/, 0x0110 /*D stroke*/, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_ParenLeft, Qt::Key_ParenRight,  0x0142 /*l stroke*/, 0x0141 /*L stroke*/, Qt::Key_Dollar, Qt::Key_ssharp },
            { Qt::Key_NumberSign, Qt::Key_Ampersand, Qt::Key_At, Qt::Key_BraceLeft, Qt::Key_BraceRight, Qt::Key_Less, Qt::Key_Greater, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Asterisk, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Turkish
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Asterisk, Qt::Key_Minus },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), 0x0131 /*dotless i*/, LOWER( Qt::Key_O ), LOWER( Qt::Key_P ), 0x011F /*g breve*/ },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ), 0x015F /*s cedilla*/, LOWER( Qt::Key_I ) },
            { Qt::Key_CapsLock, LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), LOWER( Qt::Key_Odiaeresis ), 0x00E7 /*c cedilla*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, LOWER( Qt::Key_Udiaeresis ), Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_Exclam, Qt::Key_Apostrophe, Qt::Key_AsciiCircum, Qt::Key_Plus, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question, Qt::Key_Underscore },
            { Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, 0x011E /*G breve*/ },
            { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, 0x015E /*S cedilla*/, 0x0130 /*I dot*/ },
            { Qt::Key_Shift, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Odiaeresis, 0x00C7 /*C cedilla*/, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Udiaeresis, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_QuoteDbl },
            { Qt::Key_At, Qt::Key_registered, LOWER( Qt::Key_Eacute ), Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, LOWER( Qt::Key_I ), Qt::Key_AsciiTilde },
            { LOWER( Qt::Key_AE ), 0x20AC /*Euro*/, Qt::Key_Dollar, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_NumberSign },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_copyright, Qt::Key_ssharp, Qt::Key_mu, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Space, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
},

// Chinese
{
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Minus, Qt::Key_Equal },
            { LOWER( Qt::Key_Q ), LOWER( Qt::Key_W ), LOWER( Qt::Key_E ), LOWER( Qt::Key_R ), LOWER( Qt::Key_T ), LOWER( Qt::Key_Y ), LOWER( Qt::Key_U ), LOWER( Qt::Key_I ), LOWER( Qt::Key_O ), LOWER( Qt::Key_P ) },
            { LOWER( Qt::Key_A ), LOWER( Qt::Key_S ), LOWER( Qt::Key_D ), LOWER( Qt::Key_F ), LOWER( Qt::Key_G ), LOWER( Qt::Key_H ), LOWER( Qt::Key_J ), LOWER( Qt::Key_K ), LOWER( Qt::Key_L ) },
            { LOWER( Qt::Key_Z ), LOWER( Qt::Key_X ), LOWER( Qt::Key_C ), LOWER( Qt::Key_V ), LOWER( Qt::Key_B ), LOWER( Qt::Key_N ), LOWER( Qt::Key_M ), Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_Space, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
    {
        {
        }
    }, // no upper-case
    {
        {
            { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_acute, Qt::Key_QuoteLeft },
            { Qt::Key_At, Qt::Key_Exclam, Qt::Key_QuoteDbl, Qt::Key_NumberSign, Qt::Key_Dollar, Qt::Key_Percent, Qt::Key_Ampersand, Qt::Key_Slash, Qt::Key_ParenLeft, Qt::Key_ParenRight, Qt::Key_Equal, Qt::Key_Question },
            { Qt::Key_degree, Qt::Key_twosuperior, Qt::Key_threesuperior, Qt::Key_sterling, Qt::Key_brokenbar, Qt::Key_notsign, Qt::Key_BraceLeft, Qt::Key_BracketLeft, Qt::Key_BracketRight, Qt::Key_BraceRight, Qt::Key_Backslash, Qt::Key_AsciiTilde },
            { Qt::Key_Less, Qt::Key_Greater, Qt::Key_registered, Qt::Key_copyright, 0x20AC /*Euro*/, Qt::Key_Bar, Qt::Key_onequarter, Qt::Key_onehalf, Qt::Key_threequarters, Qt::Key_guillemotleft, Qt::Key_guillemotright, Qt::Key_Backspace },
            { Qt::Key_Mode_switch, Qt::Key_Asterisk, Qt::Key_Underscore, Qt::Key_Space, Qt::Key_Semicolon, Qt::Key_Colon, Qt::Key_Left, Qt::Key_Right, Qt::Key_Return }
        }
    },
}
