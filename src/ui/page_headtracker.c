#include "page_headtracker.h"

#include <minIni.h>

#include "common.hh"
#include "core/app_state.h"
#include "core/settings.h"
#include "ht.h"
#include "page_common.h"
#include "ui/ui_style.h"

static btn_group_t btn_group;

static lv_coord_t col_dsc[] = {106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, 40, 26, 26, 26, 40, LV_GRID_TEMPLATE_LAST};
static lv_obj_t *label_cali;
static lv_obj_t *label_center;
static lv_timer_t *timer;
static lv_obj_t *pan;
static lv_obj_t *tilt;
static lv_obj_t *roll;
static slider_group_t slider_group;
bool angle_slider_selected;

static void update_visibility() {
    slider_enable(&slider_group, g_setting.ht.enable);

    if (g_setting.ht.enable) {
        lv_obj_clear_state(label_cali, STATE_DISABLED);
        lv_obj_clear_state(label_center, STATE_DISABLED);

        lv_obj_add_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_add_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        lv_obj_add_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
    } else {
        lv_obj_add_state(label_cali, STATE_DISABLED);
        lv_obj_add_state(label_center, STATE_DISABLED);

        lv_obj_clear_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
    }
}

static lv_obj_t *page_headtracker_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 780, 600);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 62, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 780, 600);

    create_text(NULL, section, false, "Head Tracker:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 780, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&btn_group, cont, 2, "Tracking", "On", "Off", "", "", 0);

    label_cali = create_label_item(cont, "Calibrate", 1, 1, 1);

    label_center = create_label_item(cont, "Set Center", 1, 2, 1);

    create_slider_item(&slider_group, cont, "Max Angle", 360, g_setting.ht.max_angle, 3);
    lv_slider_set_range(slider_group.slider, 0, 360);

    create_label_item(cont, "< Back", 1, 4, 1);

    btn_group_set_sel(&btn_group, !g_setting.ht.enable);

    create_label_item(cont, "Pan", 1, 6, 1);
    pan = lv_bar_create(cont);
    lv_bar_set_range(pan, 1000, 2000);
    lv_obj_set_size(pan, 320, 16);
    lv_obj_center(pan);
    lv_bar_set_value(pan, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(pan, lv_color_hex(TEXT_COLOR_DEFAULT), LV_PART_MAIN);
    lv_obj_set_style_radius(pan, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(pan, lv_color_make(0, 0x80, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(pan, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(pan, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 6, 1);

    create_label_item(cont, "Tilt", 1, 7, 1);
    tilt = lv_bar_create(cont);
    lv_bar_set_range(tilt, 1000, 2000);
    lv_obj_set_size(tilt, 320, 16);
    lv_obj_center(tilt);
    lv_bar_set_value(tilt, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(tilt, lv_color_hex(TEXT_COLOR_DEFAULT), LV_PART_MAIN);
    lv_obj_set_style_radius(tilt, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(tilt, lv_color_make(0, 0x80, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(tilt, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(tilt, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 7, 1);

    create_label_item(cont, "Roll", 1, 8, 1);
    roll = lv_bar_create(cont);
    lv_bar_set_range(roll, 1000, 2000);
    lv_obj_set_size(roll, 320, 16);
    lv_obj_center(roll);
    lv_bar_set_value(roll, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(roll, lv_color_hex(TEXT_COLOR_DEFAULT), LV_PART_MAIN);
    lv_obj_set_style_radius(roll, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roll, lv_color_make(0, 0x80, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(roll, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(roll, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 8, 1);

    update_visibility();
    return page;
}

static void ht_angle_inc(void) {
    int32_t value = 0;
    char buf[5];

    value = lv_slider_get_value(slider_group.slider);
    if (value < 360)
        value += 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);

    sprintf(buf, "%d", value);
    lv_label_set_text(slider_group.label, buf);

    ht_set_maxangle(value);

    g_setting.ht.max_angle = value;
}

static void ht_angle_dec(void) {
    int32_t value = 0;
    char buf[5];

    value = lv_slider_get_value(slider_group.slider);
    if (value > 0)
        value -= 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);

    sprintf(buf, "%d", value);
    lv_label_set_text(slider_group.label, buf);

    ht_set_maxangle(value);

    g_setting.ht.max_angle = value;
}

static void page_headtracker_exit_slider() {
    app_state_push(APP_STATE_SUBMENU);
    lv_obj_add_style(slider_group.slider, &style_silder_main, LV_PART_MAIN);
    ini_putl("ht", "max_angle", g_setting.ht.max_angle, SETTING_INI);
    angle_slider_selected = false;
}

static void page_headtracker_on_roller(uint8_t key) {
    if (angle_slider_selected == false) {
        return;
    }

    if (key == DIAL_KEY_UP) {
        ht_angle_dec();
    } else if (key == DIAL_KEY_DOWN) {
        ht_angle_inc();
    }
}

static void page_headtracker_on_click(uint8_t key, int sel) {
    if (sel == 0) {
        btn_group_toggle_sel(&btn_group);
        g_setting.ht.enable = btn_group_get_sel(&btn_group) == 0;
        settings_put_bool("ht", "enable", g_setting.ht.enable);
        if (g_setting.ht.enable)
            ht_enable();
        else
            ht_disable();

        update_visibility();
    } else if (sel == 1) {
        lv_label_set_text(label_cali, "Calibrating...");
        lv_timer_handler();
        ht_calibrate();
        lv_label_set_text(label_cali, "Re-calibrate");
        lv_timer_handler();
    } else if (sel == 2) {
        ht_set_center_position();
    } else if (sel == 3) {
        if (angle_slider_selected) {
            page_headtracker_exit_slider();
        } else {
            app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
            lv_obj_add_style(slider_group.slider, &style_silder_select, LV_PART_MAIN);
            angle_slider_selected = true;
        }
    }
}

static void page_headtracker_timer(struct _lv_timer_t *timer) {
    int16_t *channels = ht_get_channels();
    lv_bar_set_value(pan, channels[0], LV_ANIM_OFF);
    lv_bar_set_value(tilt, channels[1], LV_ANIM_OFF);
    lv_bar_set_value(roll, channels[2], LV_ANIM_OFF);
}

static void page_headtracker_enter() {
    lv_slider_set_value(slider_group.slider, g_setting.ht.max_angle, LV_ANIM_OFF);
    timer = lv_timer_create(page_headtracker_timer, 50, NULL);
    lv_timer_set_repeat_count(timer, -1);
    angle_slider_selected = false;
}

static void page_headtracker_exit() {
    if (angle_slider_selected) {
        page_headtracker_exit_slider();
    }
    lv_timer_del(timer);
}

page_pack_t pp_headtracker = {
    .p_arr = {
        .cur = 0,
        .max = 5,
    },
    .name = "Head Tracker",
    .create = page_headtracker_create,
    .enter = page_headtracker_enter,
    .exit = page_headtracker_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_headtracker_on_roller,
    .on_click = page_headtracker_on_click,
    .on_right_button = NULL,
};