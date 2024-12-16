/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_hello(lv_ui *ui)
{
	//Write codes hello
	ui->hello = lv_obj_create(NULL);
	lv_obj_set_size(ui->hello, 320, 240);
	lv_obj_set_scrollbar_mode(ui->hello, LV_SCROLLBAR_MODE_OFF);

	//Write style for hello, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->hello, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes hello_label_1
	ui->hello_label_1 = lv_label_create(ui->hello);
	lv_label_set_text(ui->hello_label_1, "Hello Robomaster！" LV_SYMBOL_SHUFFLE "  ");
	lv_label_set_long_mode(ui->hello_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->hello_label_1, 14, 46);
	lv_obj_set_size(ui->hello_label_1, 293, 147);

	//Write style for hello_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->hello_label_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->hello_label_1, &lv_font_ArchitectsDaughter_32, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->hello_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->hello_label_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->hello_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->hello_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes hello_bar_1
	ui->hello_bar_1 = lv_bar_create(ui->hello);
	lv_obj_set_style_anim_time(ui->hello_bar_1, 1000, 0);
	lv_bar_set_mode(ui->hello_bar_1, LV_BAR_MODE_NORMAL);
	lv_bar_set_range(ui->hello_bar_1, 0, 100);
	lv_bar_set_value(ui->hello_bar_1, 50, LV_ANIM_OFF);
	lv_obj_set_pos(ui->hello_bar_1, 112, 178);
	lv_obj_set_size(ui->hello_bar_1, 90, 20);

	//Write style for hello_bar_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->hello_bar_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->hello_bar_1, lv_color_hex(0x393c41), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->hello_bar_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->hello_bar_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->hello_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for hello_bar_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->hello_bar_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->hello_bar_1, lv_color_hex(0x00a1b5), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->hello_bar_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->hello_bar_1, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//The custom code of hello.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->hello);

}
