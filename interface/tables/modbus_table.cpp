/*
 * modbus_table.cpp
 *
 *  Created on: 15 янв. 2020 г.
 *      Author: V.Striukov
 */

#include "interface.h"
#include "modbus_table.h"

#include "comm/inter_comm.h"


#define TABLE_QUANTITY                      2
#define ROW_PER_COL                         8
#define COL_PER_ROW                         3

#define CANVAS_WIDTH                        320
#define CANVAS_HEIGHT                       205

#define CELL_ADDR_WIDTH                     39
#define CELL_VALU_WIDTH                     80
#define CELL_FRMT_WIDTH                     39

#define ROW_WIDTH                           ( CELL_ADDR_WIDTH + \
                                              CELL_VALU_WIDTH + \
                                              CELL_FRMT_WIDTH + 2 )

#define ROW_TITLE_HEIGHT                    23
#define ROW_DATA_HEIGHT                     ( ROW_TITLE_HEIGHT - 1 )

#define COL_HEIGHT                          ( ROW_TITLE_HEIGHT + 2 + \
                                              ROW_DATA_HEIGHT * ROW_PER_COL )

#define ROW_TITLE_FONT                      g_pFontWDoslike15pt
#define ROW_TITLE_OUTLINE_COLOR             0x00700099
#define ROW_TITLE_FILL_COLOR                ROW_TITLE_OUTLINE_COLOR
#define ROW_TITLE_TEXT_COLOR                0x00CCCCFF                    //0x00400044

#define ROW_DATA_FONT                       g_pFontWTahoma12pt
#define ROW_DATA_OUTLINE_COLOR              ROW_TITLE_OUTLINE_COLOR
#define ROW_DATA_FILL_COLOR                 0x00300033
#define ROW_DATA_TEXT_COLOR                 0x00CCCCFF

#define TABLE_TITLE_CELL_COLOR              ROW_TITLE_OUTLINE_COLOR
#define TABLE_BACKGROUND_COLOR              ROW_DATA_FILL_COLOR
#define TABLE_CELL_TEXT_COLOR               0x00CCCCFF //0x0083919f
#define TABLE_CELL_BORDER_COLOR             ROW_TITLE_OUTLINE_COLOR  //0x008000FF

#define SELECTION_COLOR                     ClrCyan

/****
 *  Widget Enum
 */
enum
{
    Cvs_Start,

    Cvs_Main = Cvs_Start,

    Cvs_Table_Border_1,
    Cvs_Table_Border_2,

    Cvs_Title_Underline_1,
    Cvs_Title_Underline_2,

    Cvs_Addr_Cell_1,
    Cvs_Valu_Cell_1,
    Cvs_Frmt_Cell_1,

    Cvs_Frmt_Cell_2,
    Cvs_Addr_Cell_2,
    Cvs_Valu_Cell_2,

    Cvs_Cell_Begin,
    Cvs_Cell = Cvs_Cell_Begin,
    Cvs_Cell_End = Cvs_Cell + TABLE_QUANTITY * COL_PER_ROW * ROW_PER_COL,
    Cvs_Cell_Quantity = Cvs_Cell_End - Cvs_Cell_Begin,

    Cvs_Cell_Conf,

    Cvs_End = Cvs_Cell_Conf + 1,

    Btn_Start,

    Button_Cell_1 = Btn_Start,
    Button_Cell_2,
    Button_Cell_3,
    Button_Cell_4,
    Button_Cell_5,
    Button_Cell_6,
    Button_Cell_7,
    Button_Cell_8,
    Button_Cell_9,
    Button_Cell_10,
    Button_Cell_11,
    Button_Cell_12,
    Button_Cell_13,
    Button_Cell_14,
    Button_Cell_15,
    Button_Cell_16,

    Btn_End,

    Btn_Quantity = Btn_End - Btn_Start,
};


/****
 *  To iterate through row
 */
enum {
    Column_First,

    Column_Address = Column_First,
    Column_Value,
    Column_Format,

    Column_Last,

    Column_Quantity = Column_Last,
};


static const char *column_captions[] = {
    "Addr", "Value", "Fmt",
};

typedef struct {
    uint16_t        x_pos;
    uint16_t        width;
    uint16_t        x_mid;
} col_t;


static const col_t column[Column_Last] = {
    {
        .x_pos  = 1,
        .width  = CELL_ADDR_WIDTH,
        .x_mid = 1 + CELL_ADDR_WIDTH / 2,
    },
    {
        .x_pos = CELL_ADDR_WIDTH + 1,
        .width = CELL_VALU_WIDTH,
        .x_mid = CELL_ADDR_WIDTH + 1 + CELL_VALU_WIDTH / 2,
    },
    {
        .x_pos = CELL_ADDR_WIDTH + CELL_VALU_WIDTH + 1,
        .width = CELL_FRMT_WIDTH,
        .x_mid = CELL_ADDR_WIDTH + CELL_VALU_WIDTH + 1 + CELL_FRMT_WIDTH / 2,
    }
};


static uint16_t         start_reg_id;


/****
 *  Constructor
 */
ModbusTable::ModbusTable(uint8_t ucID, Widget *parent, uint16_t coord_x, uint16_t coord_y)
            :Widget(ucID, parent, coord_x, coord_y, CANVAS_WIDTH, CANVAS_HEIGHT)
{
    Canvas                      *cvs;
    uint16_t                    gap_x = 0;
    uint16_t                    gap_y = 0;
    uint16_t                    cell_offset;
    uint16_t                    cell_id = 0;

#if 0
    char                        string[16];
    uint16_t                    btn_id = 0;
#endif

    this->active_cell_id = -1;
    this->buttons = new RectangularButton * [Btn_Quantity];

    /* First prepare table background */
    for( uint8_t table_id = 0; table_id < TABLE_QUANTITY; table_id ++ )
    {
        gap_x  = ROW_WIDTH * table_id;
        gap_y  = 0;

        cvs = new Canvas(Cvs_Table_Border_1 + table_id, this, gap_x, gap_y,
                         ROW_WIDTH, COL_HEIGHT);
        cvs->ulStyle = CANVAS_STYLE_OUTLINE;
        cvs->ulOutlineColor = ROW_TITLE_OUTLINE_COLOR;

        cvs = new Canvas(Cvs_Title_Underline_1 + table_id, this, gap_x,
                         ROW_TITLE_HEIGHT, ROW_WIDTH, 1);
        cvs->ulStyle = CANVAS_STYLE_OUTLINE;
        cvs->ulOutlineColor = ROW_TITLE_OUTLINE_COLOR;

        for( uint8_t col = Column_First; col < Column_Last; col++ )
        {
            cell_offset = gap_x + column[col].x_pos;
            gap_y = 1;

            cvs = new Canvas(Cvs_Addr_Cell_1 + col + COL_PER_ROW * table_id,
                             this, cell_offset, gap_y, column[col].width,
                             ROW_TITLE_HEIGHT - 1);
            cvs->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |
                           CANVAS_STYLE_TEXT;
            cvs->ulOutlineColor = ROW_TITLE_OUTLINE_COLOR;
            cvs->ulFillColor = ROW_TITLE_FILL_COLOR;
            cvs->ulTextColor = ROW_TITLE_TEXT_COLOR;
            cvs->pFont = ROW_TITLE_FONT;
            cvs->pcText = (char *) column_captions[col];

            gap_y += ROW_TITLE_HEIGHT;

            for( uint8_t row_id = 0; row_id < ROW_PER_COL; row_id++ )
            {
                cvs = new Canvas(Cvs_Cell + cell_id, this, cell_offset, gap_y,
                                 column[col].width, ROW_DATA_HEIGHT);
                cvs->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |
                               CANVAS_STYLE_TEXT | CANVAS_STYLE_RISING_EDGE;
                cvs->ulOutlineColor = ROW_DATA_OUTLINE_COLOR;
                cell_id++;

                gap_y += ROW_DATA_HEIGHT;
            }
        }
    }

    for( uint8_t btn = Btn_Start, btn_idx = 0; btn < Btn_End; btn++, btn_idx++ )
    {
        cell_offset = ROW_WIDTH * ((btn_idx >= 8) ? 1 : 0) + column[Column_Address].x_pos;
        gap_y = ROW_TITLE_HEIGHT + 1 + (ROW_DATA_HEIGHT * (btn_idx - ((btn_idx >= 8) ? 8 : 0)));

        this->buttons[btn_idx] = new RectangularButton(btn, this, cell_offset,
                                                       gap_y, 158, ROW_DATA_HEIGHT);
        this->buttons[btn_idx]->ulStyle =  RB_STYLE_FALLING_EDGE;
        this->buttons[btn_idx]->ulOutlineColor = SELECTION_COLOR;
    }

    cvs = new Canvas(Cvs_Main, this, coord_x, coord_y, CANVAS_WIDTH, CANVAS_HEIGHT);
    cvs->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}


void
ModbusTable::page_scroll( uint8_t scroll_act )
{
    uint16_t        offset_calculated;
    uint16_t        ofsset_max;
    uint16_t        scroll_mode = app_data_item_get(APPDATA_IFACE_SCROLL_MODE);
    uint16_t        scroll_val = (scroll_mode == Scroll_Mode_Reg) ? 1 : 8;


    offset_calculated = app_data_item_adjust(APPDATA_COMM_REG_SCROLL_VAL,
                       ( scroll_act == Prev ) ? -scroll_val : scroll_val);
    ofsset_max = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN) - 1;
    if( offset_calculated > ofsset_max )
        offset_calculated = ofsset_max;

    if( scroll_mode == Scroll_Mode_Page )
    {
        offset_calculated -= offset_calculated % 8;
    }

    app_data_item_set(APPDATA_COMM_REG_SCROLL_VAL, offset_calculated);
}


uint8_t
ModbusTable::at_left_limit( void )
{
    uint8_t     result;

    // Ugly hack to prevent window from scrolling while in setup menu
    if( !this->prop_popup )
    {
        result = app_data_item_get(APPDATA_COMM_REG_SCROLL_VAL) == 0 ? true : false;
    }
    else
    {
        result = true;
    }
    return result;
}


uint8_t
ModbusTable::at_right_limit( void )
{
    uint8_t         result;
    uint16_t        align = 0;
    uint16_t        scroll_mode = app_data_item_get(APPDATA_IFACE_SCROLL_MODE);
    uint16_t        initial_offset_value = app_data_item_get(APPDATA_COMM_REG_SCROLL_VAL);
    uint16_t        max_offset = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN) - 1;

    // Ugly hack to prevent window from scrolling while in setup menu
    if( !this->prop_popup )
    {
        if( scroll_mode == Scroll_Mode_Page )
        {
            align = max_offset % 8;
        }
        result = initial_offset_value == (max_offset - align);
    }
    else
    {
        result = true;
    }
    return result;
}


/****
 *  Table render
 */
void
ModbusTable::table_content_render( tContext *context )
{
    char                string[16];
    uint16_t            pos_x;
    uint16_t            pos_y;
    uint16_t            inter_comm_reg_id;

    uint16_t            inter_comm_reg_addr = inter_comm_start_addr_get();
    uint16_t            offset = app_data_item_get(APPDATA_COMM_REG_SCROLL_VAL);
    uint16_t            fmt;
    uint16_t            fmt_def = app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);;

    uint16_t            btn_2_disp_quan;


    start_reg_id = inter_comm_reg_addr;


    if( app_data_item_get(APPDATA_IFACE_SCROLL_MODE) == Scroll_Mode_Page )
    {
        offset -= offset % 8;
    }

    for( uint8_t table = 0; table < TABLE_QUANTITY; table++ )
    {
        for( uint8_t col = 0; col < COL_PER_ROW; col++ )
        {
            for( uint8_t row = 0; row < ROW_PER_COL; row++ )
            {
                inter_comm_reg_id = row + table * ROW_PER_COL + offset;

                if( inter_comm_reg_id < app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN))
                {
                    if( inter_comm_data_has_format(inter_comm_reg_id) )
                    {
                        fmt = inter_comm_data_format_get(inter_comm_reg_id);
                    }
                    else
                    {
                        fmt = fmt_def;
                    }

                    switch( col )
                    {
                    case Column_Address:
                        usprintf(string, "%d", start_reg_id + inter_comm_reg_id);
                        break;

                    case Column_Value:
                        if( fmt == Format_Int )
                        {
                            usprintf(string, "%d", (int16_t) InterCommDataGet(inter_comm_reg_id));
                        }
                        else
                        if( fmt == Format_Uint )
                        {
                            usprintf(string, "%d", InterCommDataGet(inter_comm_reg_id));
                        }
                        else
                        if( fmt == Format_Hex )
                        {
                            usprintf(string, "0x%04x", InterCommDataGet(inter_comm_reg_id));
                        }
                        else
                        {
                            usprintf(string, "fmt_unknwn", InterCommDataGet(inter_comm_reg_id));
                        }
                        break;

                    case Column_Format:
                        app_data_str_from_val(APPDATA_IFACE_DEFAULT_FORMAT, fmt, string);
                        break;

                    default:
                        usprintf(string, "Err", InterCommDataGet(inter_comm_reg_id));
                        break;
                    }

                    pos_x = column[col].x_mid + table * ROW_WIDTH;
                    pos_y = ROW_TITLE_HEIGHT + row * ROW_DATA_HEIGHT + GrStringHeightGet(context) - 1;

                    GrContextFontSet(context, ROW_DATA_FONT);

                    GrContextForegroundSet(context, ROW_DATA_TEXT_COLOR);
                    GrStringDrawCenteredL(context, string, strlen(string), pos_x, pos_y, -1);
                }
            }
        }
    }

    btn_2_disp_quan  = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN) - offset;

    if( btn_2_disp_quan >= Btn_Quantity )
    {
        btn_2_disp_quan = Btn_Quantity;
    }

    for( uint8_t idx = 0; idx <= Btn_Quantity; idx++ )
    {
        if( idx < btn_2_disp_quan )
        {
            this->buttons[idx]->ulStyle &= ~RB_STYLE_DISABLED;
        }
        else
        {
            this->buttons[idx]->ulStyle |=  RB_STYLE_DISABLED;
        }
    }

}


void
ModbusTable::popup_handle( void )
{
    /*
     *  Disabling buttons in this function could conflict
     *  with similar operation in @table_content_render(-)
     *  so call to this function before calling to @table_content_render
     *  above
     *
     *  TODO: move button disabling to separate function, that decides
     *        button_status, based on ORed button state
     */
    if( !this->prop_popup )
    {
        this->active_cell_id = -1;
    }

    if( this->active_cell_id == -1 )
    {
        for( uint8_t idx = 0; idx < Btn_Quantity; idx++ )
        {
            this->buttons[idx]->ulStyle &= ~RB_STYLE_DISABLED;

            if( this->buttons[idx]->ulStyle & RB_STYLE_PRESSED )
            {
                this->buttons[idx]->ulStyle |= RB_STYLE_OUTLINE;
            }
            else
            {
                this->buttons[idx]->ulStyle &= ~RB_STYLE_OUTLINE;
            }
        }
    }
    else
    {
        for( uint8_t idx = 0; idx <= Btn_Quantity; idx++ )
        {
            // Disable buttons under popup window
            if( idx / 8 != this->active_cell_id / 8 )
            {
                this->buttons[idx]->ulStyle |= RB_STYLE_DISABLED;
            }

            if( idx != this->active_cell_id )
            {
                if( this->buttons[idx]->ulStyle & RB_STYLE_PRESSED )
                {
                    this->buttons[idx]->ulStyle |= RB_STYLE_OUTLINE;
                }
                else
                {
                    this->buttons[idx]->ulStyle &= ~RB_STYLE_OUTLINE;
                }
            }
            else
            {
                this->buttons[idx]->ulStyle |= RB_STYLE_OUTLINE;
            }
        }
    }
}


/****
 *  Message Handler
 */
long
ModbusTable::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                         unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        context;
    int8_t          btn_idx;
    uint16_t        offset = app_data_item_get(APPDATA_COMM_REG_SCROLL_VAL);


    switch( ulMsg )
    {
    case WIDGET_MSG_UPDATE:
        /*
         * Notify parent
         */
        Widget::ParentNotify(this, WIDGET_NOTIFY_UPDATE);
        break;

    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&context);
        GrContextClipRegionSet(&context, &sPosition);

        GrContextFontSet(&context, g_pFontWTahoma13pt);
        GrContextForegroundSet(&context, SELECTION_COLOR);
        GrContextBackgroundSet(&context, TABLE_BACKGROUND_COLOR);

        switch( ulParam1 )
        {
        case Cvs_Main:
            /*
             *  XXX: DO NOT SWAP @popup_handle with @table_content_render,
             *       Until TODO in @popup_hanlde is done!!!
             */
            this->popup_handle();
            this->table_content_render(&context);
            break;

        default:
            return 0;
        }
        break;

    case WIDGET_NOTIFY_BTN_PRESSED:
        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);
        GrContextInit(&context);

        GrContextClipRegionSet(&context, &sPosition);

        btn_idx = ulParam1 - Btn_Start;

        switch( ulParam1 )
        {
        case Button_Cell_1:
        case Button_Cell_2:
        case Button_Cell_3:
        case Button_Cell_4:
        case Button_Cell_5:
        case Button_Cell_6:
        case Button_Cell_7:
        case Button_Cell_8:
            if( !this->prop_popup )
            {
                this->prop_popup = new WindowCellSetupPopup(Cvs_Cell_Conf, this, btn_idx + offset, 159, 2);
            }
            else
            {
                if( this->active_cell_id == btn_idx )
                {
                    this->prop_popup->Destroy();
                    this->prop_popup = 0;
                }
                else
                {
                    this->prop_popup->new_icomm_addr_set(btn_idx + offset);
                }
            }
            this->active_cell_id = btn_idx;
            break;

        case Button_Cell_9:
        case Button_Cell_10:
        case Button_Cell_11:
        case Button_Cell_12:
        case Button_Cell_13:
        case Button_Cell_14:
        case Button_Cell_15:
        case Button_Cell_16:
            if( !this->prop_popup )
            {
                this->prop_popup = new WindowCellSetupPopup(Cvs_Cell_Conf, this, btn_idx + offset, 1, 2);
            }
            else
            {
                if( this->active_cell_id == btn_idx )
                {
                    this->prop_popup->Destroy();
                    this->prop_popup = 0;
                }
                else
                {
                    this->prop_popup->new_icomm_addr_set(btn_idx + offset);
                }
            }
            this->active_cell_id = btn_idx;
            break;

        default:
            return 0;
        }
        break;

    case WIDGET_NOTIFY_DESTROY:
        switch( ulParam1 )
        {
        case Cvs_Cell_Conf:
            if( this->prop_popup )
            {
                this->prop_popup->Destroy();
                this->prop_popup = 0;
            }
            break;

        default:
            break;
        }
        break;

    default:
        return 0;
    }

    return 1;
}



/****
 *  Destructor
 */
ModbusTable::~ModbusTable(void)
{
    if( this->prop_popup )
    {
        this->prop_popup->Destroy();
        this->prop_popup = 0;
    }

    delete[] this->buttons;
}
