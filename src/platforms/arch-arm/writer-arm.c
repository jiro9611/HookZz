/**
 *    Copyright 2017 jmpews
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <string.h>

#include "writer-arm.h"

#include "writer.h"
#include "zzdefs.h"

// ATTENTION !!!:
// 写 writer 部分, 需要参考, `Instrcution Set Encoding` 部分
// `witer` REF: `Instruction Set Encoding`

ZzArmWriter *zz_arm_writer_new(zpointer data_ptr) {
    ZzArmWriter *writer = (ZzArmWriter *)malloc(sizeof(ZzArmWriter));
    self->codedata = data_ptr;
    self->base = data_ptr;
    self->pc = data_ptr;
    self->size = 0;
    return writer;
}

void zz_arm_writer_reset(ZzArmWriter *self, zpointer data_ptr) {
    self->codedata = data_ptr;
    self->base = data_ptr;
    self->pc = data_ptr;
    self->size = 0;
}

void zz_arm_writer_put_bytes(ZzArmWriter *self, zbyte *data, zuint data_size) {
    memcpy(self->codedata, data, data_size);
    self->codedata = (zpointer)self->codedata + data_size;
    self->pc += data_size;
    self->size += data_size;
}

void zz_arm_writer_put_instruction(ZzArmWriter *self, zuint32 insn) {
    *(zuint32 *)(self->codedata) = insn;
    self->codedata = (zpointer)self->codedata + sizeof(zuint32);
    self->pc += 4;
    self->size += 4;
}

void zz_arm_writer_put_b_imm(ZzArmWriter *self, zuint32 imm) {
    zz_arm_writer_put_instruction(self, 0xea000000 | (imm / 4) & 0xffffff);
}

void zz_arm_writer_put_ldr_reg_address(ZzArmWriter *self, arm_reg reg,
                                       zaddr address) {
    zz_arm_writer_put_ldr_reg_reg_imm(self, reg, ARM_REG_PC, 0);
    zz_arm_writer_put_b_imm(self, 0x4);
    zz_arm_winter_put_bytes(self, (zpointer)&address, sizeof(zpointer));
}

void zz_arm_writer_put_ldr_reg_reg_imm(ZzArmWriter *self, arm_reg dst_reg,
                                       arm_reg src_reg, zuint32 imm) {
    ZzArmRegInfo rd, rs;

    zz_arm_reg_describe(dst_reg, &rd);
    zz_arm_reg_describe(src_reg, &rs);

    zz_arm_writer_put_instruction(self, 0xe5900000 | rd.index << 12 |
                                            rs.index << 16 |
                                            (imm & GUM_INT12_MASK));
}

void zz_arm_writer_put_add_reg_reg_imm(ZzArmWriter *self, arm_reg dst_reg,
                                       arm_reg src_reg, zuint32 imm) {
    ZzArmRegInfo rd, rs;

    zz_arm_reg_describe(dst_reg, &rd);
    zz_arm_reg_describe(src_reg, &rs);

    zz_arm_writer_put_instruction(self, 0xe2800000 | rd.index << 12 |
                                            rs.index << 16 |
                                            (imm & GUM_INT12_MASK));
}