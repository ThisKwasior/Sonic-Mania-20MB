#pragma once

#define FP_4_4                  int8_t
#define FP_4_4_STEP             (int8_t)(0x10)
#define FLOAT_TO_FP_4_4(x)      (FP_4_4)(x*FP_4_4_STEP)
#define FP_4_4_TO_FLOAT(x)      (float)(x/(float)FP_4_4_STEP)

#define FP_8_8                  int16_t
#define FP_8_8_STEP             (int16_t)(0x100)
#define FLOAT_TO_FP_8_8(x)      (FP_8_8)(x*FP_8_8_STEP)
#define FP_8_8_TO_FLOAT(x)      (float)(x/(float)FP_8_8_STEP)