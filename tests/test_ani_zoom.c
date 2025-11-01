#ifndef TEST_ANI_ZOOM_C
#define TEST_ANI_ZOOM_C

#define UNITY_DEBUG 0

#include "unity.h"
#include "bmlist.h"
#include "leddrv.h"
#include <string.h>


#define ZOOM_STEPS 21
static const int expected_zoom_out_offset[ZOOM_STEPS][LED_COLS/2] = {
	{20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, },
	{19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, },
	{18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, },
	{17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, },
	{16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, },
	{15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, },
	{14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, },
	{13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, },
	{12, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, },
	{11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 21, },
	{10, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, },
	{9, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, },
	{8, 9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, },
	{7, 8, 9, 9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, },
	{6, 7, 8, 9, 9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, },
	{5, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, },
	{4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 21, },
	{3, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21, 21, },
	{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 21, },
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 21, },
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, },
};

static bm_t bm;

/* animation functions under test */
extern void ani_zoom(bm_t *bm, uint16_t *fb, int step);

static bm_t make_bm(void)
{
	bm.width = LED_COLS;
	bm.buf = malloc(LED_COLS * sizeof(uint16_t));
	int col_offset;
	for (col_offset = 0; col_offset < bm.width/2; col_offset += 2) {
		bm.buf[col_offset] = (uint16_t)(0x7FF);
		bm.buf[LED_COLS-1-col_offset] = (uint16_t)(0x7FF);
		if (col_offset+1 < bm.width/2) {
            bm.buf[col_offset+1] = (uint16_t)(0x000);
            bm.buf[LED_COLS-1-col_offset-1] = (uint16_t)(0x000);
		}
    }
	bm.anim_step = 0;
#if UNITY_DEBUG
	// for debug output
	printf("bm: ");
	for (int i = 0; i < LED_COLS; ++i) {
		printf("%03X ", bm.buf[i]);
	}
	printf("\n\n");
#endif	/* UNITY_DEBUG */

	return bm;
}

void setUp(void) {
	bm = make_bm();
}
void tearDown(void) {
	free(bm.buf);
}

void test_ani_zoom_full_display(void)
{
	uint16_t fb[LED_COLS];
	memset(fb, 0x0, sizeof(fb));

	/* Step inside the "display all columns" interval */
    for (int step = LED_COLS/2; step < LED_COLS*3; step++) {
		ani_zoom(&bm, fb, step);

		for (int i = 0; i < LED_COLS; i++) {
			TEST_ASSERT_EQUAL_UINT16(bm.buf[i], fb[i]);
		}
	}
}

void test_ani_zoom_zoom_out(void)
{
	uint16_t fb[LED_COLS];
	memset(fb, 0x0, sizeof(fb));

    /* Steps 0 ~ LED_COLS/2-1 */
    for (int step = 0; step < LED_COLS/2; step++) {
        ani_zoom(&bm, fb, step);
#if UNITY_DEBUG
		// for debug output
		printf("Step %3d: ", step);
		for (int i = 0; i < LED_COLS; ++i) {
			printf("%03X ", fb[i]);
		}
		printf("\n");
#endif	/* UNITY_DEBUG */

		if (step == 0) {
			TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS/2 - 1], fb[LED_COLS/2 - 1]);
			TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS/2], fb[LED_COLS/2]);
			for (int i = 1; i < LED_COLS-1; i++) {
				TEST_ASSERT_EQUAL_UINT16(0, fb[i]);
			}
		} else {
			for (int i = 0; i < LED_COLS/2; i++) {
				TEST_ASSERT_EQUAL_UINT16(bm.buf[expected_zoom_out_offset[step-1][i]], fb[i]);
				TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS-1-expected_zoom_out_offset[step-1][LED_COLS/2-1-i]], fb[LED_COLS/2+i]);
			}
		}
	}
}

void test_ani_zoom_zoom_in(void)
{
	uint16_t fb[LED_COLS];
	memset(fb, 0x0, sizeof(fb));

    /* Steps LED_COLS*3 ~ LED_COLS*7/2-1 */
    for (int step = LED_COLS*3; step < LED_COLS*7/2; step++) {
        ani_zoom(&bm, fb, step);
		int zoom_offset = LED_COLS/2-1 - (step-LED_COLS*3);
#if UNITY_DEBUG
		// for debug output
		printf("Step %3d (aligned=%3d): ", step, zoom_offset);
		for (int i = 0; i < LED_COLS; ++i) {
			printf("%03X ", fb[i]);
		}
		printf("\n");
#endif	/* UNITY_DEBUG */

		if (step == LED_COLS*7/2 - 1) {
			TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS/2 - 1], fb[LED_COLS/2 - 1]);
			TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS/2], fb[LED_COLS/2]);
			for (int i = 1; i < LED_COLS-1; i++) {
				TEST_ASSERT_EQUAL_UINT16(0, fb[i]);
			}
		} else {
			for (int i = 0; i < LED_COLS/2; i++) {
				TEST_ASSERT_EQUAL_UINT16(bm.buf[expected_zoom_out_offset[zoom_offset-1][i]], fb[i]);
				TEST_ASSERT_EQUAL_UINT16(bm.buf[LED_COLS-1-expected_zoom_out_offset[zoom_offset-1][LED_COLS/2-1-i]], fb[LED_COLS/2+i]);
			}
		}
	}
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_ani_zoom_full_display);
	RUN_TEST(test_ani_zoom_zoom_out);
	RUN_TEST(test_ani_zoom_zoom_in);
	return UNITY_END();
}

#endif // TEST_ANI_ZOOM_C
