DEF_HELPER_1(bitrev, i32, i32)
DEF_HELPER_1(ff1, i32, i32)
DEF_HELPER_2(bfffo, i32, i32, i32)
DEF_HELPER_2(rol32, i32, i32, i32)
DEF_HELPER_2(ror32, i32, i32, i32)
DEF_HELPER_2(sats, i32, i32, i32)
DEF_HELPER_2(divu, void, env, i32)
DEF_HELPER_2(divs, void, env, i32)
DEF_HELPER_1(divu64, void, env)
DEF_HELPER_1(divs64, void, env)
DEF_HELPER_3(mulu32_cc, i32, env, i32, i32)
DEF_HELPER_3(muls32_cc, i32, env, i32, i32)
DEF_HELPER_3(mulu64, i32, env, i32, i32)
DEF_HELPER_3(muls64, i32, env, i32, i32)
DEF_HELPER_3(addx8_cc, i32, env, i32, i32)
DEF_HELPER_3(addx16_cc, i32, env, i32, i32)
DEF_HELPER_3(addx32_cc, i32, env, i32, i32)
DEF_HELPER_3(subx8_cc, i32, env, i32, i32)
DEF_HELPER_3(subx16_cc, i32, env, i32, i32)
DEF_HELPER_3(subx32_cc, i32, env, i32, i32)
DEF_HELPER_3(shl8_cc, i32, env, i32, i32)
DEF_HELPER_3(shl16_cc, i32, env, i32, i32)
DEF_HELPER_3(shl32_cc, i32, env, i32, i32)
DEF_HELPER_3(shr8_cc, i32, env, i32, i32)
DEF_HELPER_3(shr16_cc, i32, env, i32, i32)
DEF_HELPER_3(shr32_cc, i32, env, i32, i32)
DEF_HELPER_3(sal8_cc, i32, env, i32, i32)
DEF_HELPER_3(sal16_cc, i32, env, i32, i32)
DEF_HELPER_3(sal32_cc, i32, env, i32, i32)
DEF_HELPER_3(sar8_cc, i32, env, i32, i32)
DEF_HELPER_3(sar16_cc, i32, env, i32, i32)
DEF_HELPER_3(sar32_cc, i32, env, i32, i32)
DEF_HELPER_3(rol8_cc, i32, env, i32, i32)
DEF_HELPER_3(rol16_cc, i32, env, i32, i32)
DEF_HELPER_3(rol32_cc, i32, env, i32, i32)
DEF_HELPER_3(ror8_cc, i32, env, i32, i32)
DEF_HELPER_3(ror16_cc, i32, env, i32, i32)
DEF_HELPER_3(ror32_cc, i32, env, i32, i32)
DEF_HELPER_3(roxr8_cc, i32, env, i32, i32)
DEF_HELPER_3(roxr16_cc, i32, env, i32, i32)
DEF_HELPER_3(roxr32_cc, i32, env, i32, i32)
DEF_HELPER_3(roxl8_cc, i32, env, i32, i32)
DEF_HELPER_3(roxl16_cc, i32, env, i32, i32)
DEF_HELPER_3(roxl32_cc, i32, env, i32, i32)
DEF_HELPER_2(xflag_lt_i8, i32, i32, i32)
DEF_HELPER_2(xflag_lt_i16, i32, i32, i32)
DEF_HELPER_2(xflag_lt_i32, i32, i32, i32)
DEF_HELPER_2(set_sr, void, env, i32)
DEF_HELPER_3(movec, void, env, i32, i32)

DEF_HELPER_2(f64_to_i32, f32, env, f64)
DEF_HELPER_2(f64_to_f32, f32, env, f64)
DEF_HELPER_2(i32_to_f64, f64, env, i32)
DEF_HELPER_2(f32_to_f64, f64, env, f32)
DEF_HELPER_2(iround_f64, f64, env, f64)
DEF_HELPER_2(itrunc_f64, f64, env, f64)
DEF_HELPER_2(sqrt_f64, f64, env, f64)
DEF_HELPER_1(abs_f64, f64, f64)
DEF_HELPER_1(chs_f64, f64, f64)
DEF_HELPER_3(add_f64, f64, env, f64, f64)
DEF_HELPER_3(sub_f64, f64, env, f64, f64)
DEF_HELPER_3(mul_f64, f64, env, f64, f64)
DEF_HELPER_3(div_f64, f64, env, f64, f64)
DEF_HELPER_3(sub_cmp_f64, f64, env, f64, f64)
DEF_HELPER_2(compare_f64, i32, env, f64)

DEF_HELPER_3(mac_move, void, env, i32, i32)
DEF_HELPER_2(read_disk, void, env, i32)
DEF_HELPER_2(write_disk, void, env, i32)
DEF_HELPER_3(macmulf, i64, env, i32, i32)
DEF_HELPER_3(macmuls, i64, env, i32, i32)
DEF_HELPER_3(macmulu, i64, env, i32, i32)
DEF_HELPER_2(macsats, void, env, i32)
DEF_HELPER_2(macsatu, void, env, i32)
DEF_HELPER_2(macsatf, void, env, i32)
DEF_HELPER_2(mac_set_flags, void, env, i32)
DEF_HELPER_2(set_macsr, void, env, i32)
DEF_HELPER_2(get_macf, i32, env, i64)
DEF_HELPER_1(get_macs, i32, i64)
DEF_HELPER_1(get_macu, i32, i64)
DEF_HELPER_2(get_mac_extf, i32, env, i32)
DEF_HELPER_2(get_mac_exti, i32, env, i32)
DEF_HELPER_3(set_mac_extf, void, env, i32, i32)
DEF_HELPER_3(set_mac_exts, void, env, i32, i32)
DEF_HELPER_3(set_mac_extu, void, env, i32, i32)

DEF_HELPER_2(flush_flags, i32, env, i32)
DEF_HELPER_2(raise_exception, void, env, i32)

DEF_HELPER_3(bitfield_load, i64, i32, i32, i32)
DEF_HELPER_4(bitfield_store, void, i32, i32, i32, i64)

DEF_HELPER_3(abcd_cc, i32, env, i32, i32)
DEF_HELPER_3(sbcd_cc, i32, env, i32, i32)
