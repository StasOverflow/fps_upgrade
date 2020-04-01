/*
 * window_cell_setup_popup.cpp
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: V.Striukov
 */
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"

#include "interface.h"
#include "window_cell_setup_popup.h"


#define CANVAS_WIDTH            160
#define CANVAS_HEIGHT           198

#define BUTTON_WIDTH            75
#define BUTTON_HEIGHT           25

#define INP_BUTTON_WIDTH        29
#define INP_BUTTON_HEIGHT       24

#define DIGIT_OVF_MAGIC         0x2710
#define BTN_ENUM_TO_IDX(x)      (x - Button_Start)


extern TaskHandle_t interface_handler;

enum Canvases {
    Canvas_Main,
    Canvas_Title,
    Canvas_Static,
    Canvas_Data,

    Button_Start,

    Button_Back = Button_Start,
    Button_Apply,

    Button_1,
    Button_2,
    Button_3,
    Button_4,
    Button_5,
    Button_6,
    Button_7,
    Button_8,
    Button_9,
    Button_Clr_One,
    Button_0,
    Button_Clr_Full,

    Button_A,
    Button_B,
    Button_C,
    Button_D,
    Button_E,
    Button_F,
    Button_Plus_Minus,

    Button_Int,
    Button_Uint,
    Button_Hex,

    Button_End,

    Button_Quantity = Button_End-Button_Start,
};


/***
 *  Constructor
 */
WindowCellSetupPopup::WindowCellSetupPopup(unsigned char ucID, Widget *pParent,
                      uint16_t icomm_id, int32_t pos_x, int32_t pos_y)
                     :Widget(ucID, pParent, pos_x, pos_y,
                             CANVAS_WIDTH, CANVAS_HEIGHT)
{
    Canvas            *canvas;
    tRectangle         pos;
    uint16_t           lpos_x;
    uint16_t           lpos_y;
    uint8_t            btn_idx;

    static const char * inp_caption_def[] = {
        "1", "2", "3",
        "4", "5", "6",
        "7", "8", "9",
        "<", "0", "x",
    };

    static const char * inp_caption_ext[] = {
        "A", "B",
        "C", "D",
        "E", "F",
        "+/-"
    };

    static const char * fmt_caption[] = {
        "int",
        "uint",
        "hex"
    };

    this->setup_value = 0;
    this->input_in_process = false;
    this->minus_sign = false;

    this->comm_id       = icomm_id;
    this->buttons       = new RectangularButton*[Button_End-Button_Start];
    if( inter_comm_data_has_format(icomm_id) )
    {
        this->format = (format_t) inter_comm_data_format_get(this->comm_id);
    }
    else
    {
        this->format = (format_t) app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);
    }

    this->modify_value  = 0;

    // Cvs Main
    canvas = new Canvas(Canvas_Main, this, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas->ulStyle = CANVAS_STYLE_RISING_EDGE | CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE;
    canvas->ulFillColor    = 0x00700099;
    canvas->ulOutlineColor = ClrCyan;

    this->AbsolutePositionGet(&pos);

    // Button Back
    btn_idx = BTN_ENUM_TO_IDX(Button_Back);

    this->buttons[btn_idx] = new RectangularButton(Button_Back, this, 4, CANVAS_HEIGHT -
                                   BUTTON_HEIGHT - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
    this->buttons[btn_idx]->ulStyle = RB_STYLE_FILL         |
                                      RB_STYLE_FALLING_EDGE |
                                      RB_STYLE_TEXT;
    this->buttons[btn_idx]->ulFillColor = 0x00700099;
    this->buttons[btn_idx]->ulPressFillColor = 0x00700099;
    this->buttons[btn_idx]->ulTextColor = 0x00CCCCFF;
    this->buttons[btn_idx]->pFont = g_pFontWDoslike15pt;
    this->buttons[btn_idx]->pcText = (char *)"Close";

    // Button Apply
    btn_idx = BTN_ENUM_TO_IDX(Button_Apply);

    this->buttons[btn_idx] = new RectangularButton(Button_Apply, this, CANVAS_WIDTH - BUTTON_WIDTH - 4,
                                    CANVAS_HEIGHT - BUTTON_HEIGHT - 2, BUTTON_WIDTH, BUTTON_HEIGHT);
    this->buttons[btn_idx]->ulStyle = RB_STYLE_FILL         |
                                      RB_STYLE_FALLING_EDGE |
                                      RB_STYLE_TEXT;
    this->buttons[btn_idx]->ulFillColor = 0x00700099;
    this->buttons[btn_idx]->ulPressFillColor = 0x00700099;
    this->buttons[btn_idx]->ulTextColor = 0x00CCCCFF;
    this->buttons[btn_idx]->pFont = g_pFontWDoslike15pt;
    this->buttons[btn_idx]->pcText = (char *)"Send";

    lpos_x = 3;
    lpos_y = 1;
    for( uint8_t btn = Button_Int, idx = BTN_ENUM_TO_IDX(Button_Int), cnt = 0; btn <= Button_Hex; btn++, idx++, cnt++ )
    {
        this->buttons[idx] = new RectangularButton(btn, this, lpos_x, lpos_y, 50, 41);
        this->buttons[idx]->ulOutlineColor = ClrCyan;
        this->buttons[idx]->ulStyle = RB_STYLE_FILL         |
                                      RB_STYLE_FALLING_EDGE;
        this->buttons[idx]->ulFillColor = 0x00700099;
        this->buttons[idx]->ulPressFillColor = 0x00700099;

        if( this->format == idx )
        {
            this->buttons[idx]->ulStyle |=  RB_STYLE_OUTLINE;
        }
        else
        {
            this->buttons[idx]->ulStyle &= ~RB_STYLE_OUTLINE;
        }

        canvas = new Canvas(Canvas_Static, this, lpos_x, lpos_y, 50, 20);
        canvas->ulStyle = CANVAS_STYLE_TEXT;
        canvas->ulTextColor = 0x00CCCCFF;
        canvas->pFont = g_pFontWTahoma13pt;
        canvas->pcText = (char *) fmt_caption[cnt];

        lpos_x += 52;
    }

    for( uint8_t btn = Button_1, i = BTN_ENUM_TO_IDX(Button_1), cnt = 0; btn <= Button_Clr_Full; btn++, i++, cnt++ )
    {
        lpos_x = (cnt % 3) * (INP_BUTTON_WIDTH + 1) + 4;
        lpos_y = (cnt / 3) * (INP_BUTTON_HEIGHT+ 1) + 69;

        this->buttons[i] = new RectangularButton(btn, this, lpos_x, lpos_y,
                                                 INP_BUTTON_WIDTH, INP_BUTTON_HEIGHT);
        this->buttons[i]->ulStyle = RB_STYLE_FILL         |
                                    RB_STYLE_FALLING_EDGE |
                                    RB_STYLE_TEXT;
        this->buttons[i]->ulFillColor = 0x00700099;
        this->buttons[i]->ulPressFillColor = 0x00700099;
        this->buttons[i]->ulTextColor = 0x00CCCCFF;
        this->buttons[i]->pFont = g_pFontWDoslike15pt;
        this->buttons[i]->pcText = (char *) inp_caption_def[cnt];
    }


    for( uint8_t btn = Button_A, i = BTN_ENUM_TO_IDX(Button_A), cnt = 0; btn <= Button_Plus_Minus; btn++, i++, cnt++ )
    {
        lpos_x = (cnt % 2) * (INP_BUTTON_WIDTH + 1) + 97;
        lpos_y = (cnt / 2) * (INP_BUTTON_HEIGHT+ 1) + 69;

        this->buttons[i] = new RectangularButton(btn, this, lpos_x, lpos_y,
                                                (btn != Button_Plus_Minus ?
                                                 INP_BUTTON_WIDTH : INP_BUTTON_WIDTH * 2 + 1),
                                                 INP_BUTTON_HEIGHT);
        this->buttons[i]->ulStyle = RB_STYLE_FILL         |
                                    RB_STYLE_FALLING_EDGE |
                                    RB_STYLE_TEXT;
        this->buttons[i]->ulFillColor = 0x00700099;
        this->buttons[i]->ulPressFillColor = 0x00700099;
        this->buttons[i]->ulTextColor = 0x00CCCCFF;
        this->buttons[i]->pFont = g_pFontWDoslike15pt;
        this->buttons[i]->pcText = (char *) inp_caption_ext[cnt];
    }

    this->interface_update();

    canvas = new Canvas(Canvas_Static, this, 4, 43, 152, 23);
    canvas->ulStyle = CANVAS_STYLE_FILL;
    canvas->ulFillColor = ClrBlack;

    usprintf(string, "%d", InterCommDataGet(this->comm_id));
    canvas = new Canvas(Canvas_Title, this, 45, 43, 60, 23);
    canvas->ulStyle = CANVAS_STYLE_TEXT         |
                      CANVAS_STYLE_TEXT_RIGHT;
    canvas->pFont       = g_pFontWDoslike15pt;
    canvas->ulTextColor = ClrWhite;
    canvas->pcText = (char *) this->string;

    canvas = new Canvas(Canvas_Data, this, 0, 0, CANVAS_HEIGHT, CANVAS_WIDTH);
    canvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}


void
WindowCellSetupPopup::new_icomm_addr_set( uint16_t icomm_id )
{
    this->comm_id = icomm_id;
    if( inter_comm_data_has_format(icomm_id) )
    {
        this->format        = (format_t) inter_comm_data_format_get(this->comm_id);
    }
    else
    {
        this->format        = (format_t) app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);
    }
    this->modify_value = 0;
}


void
WindowCellSetupPopup::setup_value_modify( uint8_t new_data )
{
    int32_t         raw_value = setup_value;


//    if( this->setup_value < DIGIT_OVF_MAGIC )
    {
        switch(this->format)
        {
        case Format_Int:
            if( this->minus_sign )
            {
                raw_value = (this->setup_value * 10) - new_data;
            }
            else
            {
                raw_value = (this->setup_value * 10) + new_data;
            }
            break;

        case Format_Uint:
            raw_value = (this->setup_value * 10) + new_data;
            break;

        case Format_Hex:
            raw_value = (this->setup_value << 4) + new_data;
            break;
        }
    }

    this->setup_value = raw_value;

    this->input_in_process = true;
    this->input_action_timer = xTaskGetTickCount();
}


void
WindowCellSetupPopup::interface_update( void )
{
    if( this->input_in_process )
    {
        if( xTaskGetTickCount() - this->input_action_timer >= 15000 )
        {
            this->input_in_process = false;
        }
    }
    else
    {
        this->minus_sign = false;
        this->setup_value = 0;
    }

    // Selected format button oultine render
    for( uint8_t idx = BTN_ENUM_TO_IDX(Button_Int), btn = 0; idx <= BTN_ENUM_TO_IDX(Button_Hex); idx++, btn++ )
    {
        if( this->format == btn )
        {
            this->buttons[idx]->ulStyle |=  RB_STYLE_OUTLINE;
        }
        else
        {
            this->buttons[idx]->ulStyle &= ~RB_STYLE_OUTLINE;
        }
    }

    // Disable unused input buttons (based on format)
    switch( this->format )
    {
    case Format_Int:
        for( uint8_t btn_idx = BTN_ENUM_TO_IDX(Button_A); btn_idx <= BTN_ENUM_TO_IDX(Button_F); btn_idx++ )
        {
            this->buttons[btn_idx]->ulStyle |=  RB_STYLE_DISABLED;
        }
        this->buttons[BTN_ENUM_TO_IDX(Button_Plus_Minus)]->ulStyle &= ~RB_STYLE_DISABLED;
        break;

    case Format_Uint:
        for( uint8_t btn_idx = BTN_ENUM_TO_IDX(Button_A); btn_idx <= BTN_ENUM_TO_IDX(Button_F); btn_idx++ )
        {
            this->buttons[btn_idx]->ulStyle |=  RB_STYLE_DISABLED;
        }
        this->buttons[BTN_ENUM_TO_IDX(Button_Plus_Minus)]->ulStyle |=  RB_STYLE_DISABLED;
        break;

    case Format_Hex:
        for( uint8_t btn_idx = BTN_ENUM_TO_IDX(Button_A); btn_idx <= BTN_ENUM_TO_IDX(Button_F); btn_idx++ )
        {
            this->buttons[btn_idx]->ulStyle &= ~RB_STYLE_DISABLED;
        }
        this->buttons[BTN_ENUM_TO_IDX(Button_Plus_Minus)]->ulStyle |=  RB_STYLE_DISABLED;
        break;
    }
}


void
WindowCellSetupPopup::display_values_update( tContext & context, tRectangle & pos )
{
    char                lstring[8];     //  l stands for local
    uint16_t            value;
    TickType_t          time_curr = xTaskGetTickCount();


    if( time_curr - this->blink_timer > 333 )  // animate blinking
    {
        this->blink ^= 1;
        this->blink_timer = time_curr;
    }

    if( this->input_in_process )
    {
        GrContextForegroundSet(&context, ClrWhiteSmoke);

        if( this->blink ) // Draw text cursor
        {
            GrLineDrawH(&context, pos.sXMin + 105, pos.sXMin + 107, pos.sYMin + 48);
            GrLineDrawH(&context, pos.sXMin + 105, pos.sXMin + 107, pos.sYMin + 60);
            GrLineDrawV(&context, pos.sXMin + 106, pos.sYMin +  48, pos.sYMin + 60);
        }
        this->display_value = this->setup_value;
    }
    else
    {
        this->blink = 1;
        this->blink_timer = time_curr;
        this->display_value = InterCommDataGet(this->comm_id);
    }
    value = this->display_value;

    GrContextFontSet(&context, g_pFontWTahoma13pt);
    GrContextForegroundSet(&context, ClrWhiteSmoke);
    GrContextBackgroundSet(&context, 0x00700099);

    for( uint8_t idx = BTN_ENUM_TO_IDX(Button_Int), btn = 0; idx <= BTN_ENUM_TO_IDX(Button_Hex); idx++, btn++ )
    {
        switch( btn )
        {
        case Format_Hex:
            usprintf(lstring, "0x%04X", value);
            break;

        case Format_Int:
            usprintf(lstring, "%d", (int16_t) value);
            break;

        case Format_Uint:
            usprintf(lstring, "%d", (uint16_t) value);
            break;
        }

        switch(this->format)
        {
        case Format_Hex:
            usprintf(this->string, "0x%X", value);
            break;

        case Format_Int:
            usprintf(this->string, "%d", (int16_t) value);
            break;

        case Format_Uint:
            usprintf(this->string, "%d", (uint16_t) value);
            break;
        }

        GrStringDrawCenteredL(&context, lstring, -1, pos.sXMin + 28 + 52 *btn, pos.sYMin + 28, -1);
    }

//    if( this->setup_value >= DIGIT_OVF_MAGIC )
//    {
//        GrTransparentImageDraw(&context, g_pucimg_led_red_small, pos.sXMin + 145, pos.sYMin + 52, 1);
//    }
//    else
//    {
//        GrTransparentImageDraw(&context, g_pucimg_led_green_upgraded_small, pos.sXMin + 145, pos.sYMin + 52, 0);
//    }
}

/***
 *  Messageproc
 */
long
WindowCellSetupPopup::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                                  unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;


    switch( ulMsg )
    {
    case WIDGET_MSG_UPDATE:
        Widget::ParentNotify(this, WIDGET_NOTIFY_UPDATE);
        break;

    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);

        switch( ulParam1 )
        {
        case Canvas_Data:
            this->display_values_update(sCtx, sPosition);
            this->interface_update();

//            GrContextFontSet(&sCtx, g_pFontWTahoma12pt);
//            GrContextForegroundSet(&sCtx, ClrLime);
//            GrContextBackgroundSet(&sCtx, ClrBlack);

//            usprintf(lstring, "%d", this->input_in_process);
//            GrStringDraw(&sCtx, lstring, -1, sPosition.sXMin + 8, 50, -1);

//            usprintf(string, "%d", BTN_ENUM_TO_IDX(Button_1));
//            GrStringDraw(&sCtx, string, -1, 290, 47, -1);
            break;

        default:
            return 0;
        }
        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Button_1:
        case Button_2:
        case Button_3:
        case Button_4:
        case Button_5:
        case Button_6:
        case Button_7:
        case Button_8:
        case Button_9:
        case Button_0:
            this->setup_value_modify((ulParam1 != Button_0) ? (ulParam1 - Button_1 + 1) : 0);
            break;

        case Button_A:
        case Button_B:
        case Button_C:
        case Button_D:
        case Button_E:
        case Button_F:
            this->setup_value_modify(ulParam1 - Button_A + 10);
            break;

        case Button_Clr_Full:
            this->setup_value = 0;

            this->input_in_process = true;
            this->input_action_timer = xTaskGetTickCount();
            break;

        case Button_Clr_One:
            switch( this->format )
            {
            case Format_Hex:
                this->setup_value >>= 4;
                break;

            case Format_Int:
            case Format_Uint:
                this->setup_value /= 10;
                break;
            }

            this->input_in_process = true;
            this->input_action_timer = xTaskGetTickCount();
            break;

        case Button_Plus_Minus:
            this->setup_value = (int16_t) -this->setup_value;
            this->minus_sign ^= 1;

            this->input_in_process = true;
            this->input_action_timer = xTaskGetTickCount();
            break;

        case Button_Int:
        case Button_Uint:
        case Button_Hex:
            this->format = (format_t) (ulParam1 - Button_Int);
            inter_comm_data_format_set(this->comm_id, this->format);
            break;

        case Button_Back:
            Widget::ParentNotify(this, WIDGET_NOTIFY_DESTROY);
            break;

        case Button_Apply:
            if( this->input_in_process )
            {
                InterCommDataSet(this->comm_id, this->setup_value);
                this->input_in_process = false;
            }
            break;

        default:
            return 0;
        }
        break;

    default:
        return 0;
    }

    return 1;
}


/***
 *  Destructor
 */
WindowCellSetupPopup::~WindowCellSetupPopup(void)
{
//    for( uint8_t i = 0; i < (Button_End - Button_Start); i++ )
//    {
//        delete this->buttons[i];
//    }
    delete[] this->buttons;
}
