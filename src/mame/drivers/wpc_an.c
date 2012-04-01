/*
    Williams WPC (Alpha Numeric)
*/


#include "emu.h"
#include "cpu/m6809/m6809.h"

class wpc_an_state : public driver_device
{
public:
	wpc_an_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu")
	{ }

protected:

	// devices
	required_device<cpu_device> m_maincpu;

	// driver_device overrides
	virtual void machine_reset();
};


static ADDRESS_MAP_START( wpc_an_map, AS_PROGRAM, 8, wpc_an_state )
	AM_RANGE(0x0000, 0xffff) AM_NOP
ADDRESS_MAP_END

static INPUT_PORTS_START( wpc_an )
INPUT_PORTS_END

void wpc_an_state::machine_reset()
{
}

static DRIVER_INIT( wpc_an )
{
}

static MACHINE_CONFIG_START( wpc_an, wpc_an_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6809, 2000000)
	MCFG_CPU_PROGRAM_MAP(wpc_an_map)
MACHINE_CONFIG_END


/*-----------------
/  Dr. Dude
/------------------*/
ROM_START(dd_p7)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("dude_u6.p7", 0x00000, 0x020000, CRC(b6c35b98) SHA1(5e9d70ce40669e2f402561dc1d8aa70a8b8a2958))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x30000, "sound1", 0)
	ROM_LOAD("dude_u4.l1", 0x00000, 0x10000, CRC(3eeef714) SHA1(74dcc83958cb62819e0ac36ca83001694faafec7))
	ROM_LOAD("dude_u19.l1", 0x10000, 0x10000, CRC(dc7b985b) SHA1(f672d1f1fe1d1d887113ea6ccd745a78f7760526))
	ROM_LOAD("dude_u20.l1", 0x20000, 0x10000, CRC(a83d53dd) SHA1(92a81069c42c7760888201fb0787fa7ddfbf1658))
ROM_END

ROM_START(dd_p06)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("u6-pa6.wpc", 0x00000, 0x020000, CRC(fb72571b) SHA1(a12b32eac3141c881064e6de2f49d6d213248fde))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x30000, "sound1", 0)
	ROM_LOAD("dude_u4.l1", 0x00000, 0x10000, CRC(3eeef714) SHA1(74dcc83958cb62819e0ac36ca83001694faafec7))
	ROM_LOAD("dude_u19.l1", 0x10000, 0x10000, CRC(dc7b985b) SHA1(f672d1f1fe1d1d887113ea6ccd745a78f7760526))
	ROM_LOAD("dude_u20.l1", 0x20000, 0x10000, CRC(a83d53dd) SHA1(92a81069c42c7760888201fb0787fa7ddfbf1658))
ROM_END

/*-------------
/ Funhouse
/--------------*/
ROM_START(fh_l9)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x040000, "user2", 0)
	ROM_LOAD("funh_l9.rom", 0x00000, 0x040000, CRC(c8f90ff8) SHA1(8d200ea30a68f5e3ba1ac9232a516c44b765eb45))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl3", 0x000000, 0x20000, CRC(7f6c7045) SHA1(8c8d601e8e6598507d75b4955ccc51623124e8ab))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(fh_l9b)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x040000, "user2", 0)
	ROM_LOAD("fh_l9ger.rom", 0x00000, 0x040000, CRC(e9b32a8f) SHA1(deb77f0d025001ddcc3045b4e49176c54896da3f))
	ROM_REGION(0x010000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl3", 0x000000, 0x20000, CRC(7f6c7045) SHA1(8c8d601e8e6598507d75b4955ccc51623124e8ab))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(fh_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("u6-l3.rom", 0x00000, 0x020000, CRC(7a74d702) SHA1(91540cdc62c855b4139b202aa6ad5440b2dee141))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl2", 0x000000, 0x20000, CRC(11c8944a) SHA1(425d8da5a036c41e054d201b99856319fd5ef9e2))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(fh_l4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x040000, "user2", 0)
	ROM_LOAD("u6-l4.rom", 0x00000, 0x020000, CRC(f438aaca) SHA1(42bf75325a0e85a4334a5a710c2eddf99160ffbf))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl2", 0x000000, 0x20000, CRC(11c8944a) SHA1(425d8da5a036c41e054d201b99856319fd5ef9e2))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)

ROM_END

ROM_START(fh_l5)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x040000, "user2", 0)
	ROM_LOAD("u6-l5.rom", 0x00000, 0x020000, CRC(e2b25da4) SHA1(87129e18c60a65035ade2f4766c154d5d333696b))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl2", 0x000000, 0x20000, CRC(11c8944a) SHA1(425d8da5a036c41e054d201b99856319fd5ef9e2))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(fh_905h)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x080000, "user2", 0)
	ROM_LOAD("fh_905h.rom", 0x00000, 0x080000, CRC(445b632a) SHA1(6e277027a1d025e2b93f0d7736b414ba3a68a4f8))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("fh_u18.sl3", 0x000000, 0x20000, CRC(7f6c7045) SHA1(8c8d601e8e6598507d75b4955ccc51623124e8ab))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u15.sl2", 0x080000, 0x20000, CRC(0744b9f5) SHA1(b626601d82e6b1cf25f7fdcca31e623fc14a3f92))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("fh_u14.sl2", 0x100000, 0x20000, CRC(3394b69b) SHA1(34690688f00106b725b27a6975cdbf1e077e3bb3))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END


/*-----------------
/  Harley Davidson
/------------------*/
ROM_START(hd_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("harly_l3.rom", 0x00000, 0x020000, CRC(65f2e0b4) SHA1(a44216c13b9f9adf4161ff6f9eeceba28ef37963))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("hd_u18.rom", 0x000000, 0x20000, CRC(810d98c0) SHA1(8080cbbe0f346020b2b2b8e97015dbb615dbadb3))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("hd_u15.rom", 0x080000, 0x20000, CRC(e7870938) SHA1(b4f28146a5e7baa8522db65b41311afaf49604c6))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
ROM_END

ROM_START(hd_l1)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("u6-l1.rom", 0x00000, 0x020000, CRC(a0bdcfbf) SHA1(f906ffa2d4d04e87225bf711a07dd3bee1655a40))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("u18-sp1.rom", 0x000000, 0x20000, CRC(708aa419) SHA1(cfc2692fb3bcbacceb85021e282bfbc8dcdf8fcc))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("hd_u15.rom", 0x080000, 0x20000, CRC(e7870938) SHA1(b4f28146a5e7baa8522db65b41311afaf49604c6))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
ROM_END

/*-----------------
/  The Machine: Bride of Pinbot
/------------------*/
ROM_START(bop_l7)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("tmbopl_7.rom", 0x00000, 0x40000, CRC(773e1488) SHA1(36e8957b3903b99844a76bf15ba393b17db0db59))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(bop_l6)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("tmbopl_6.rom", 0x00000, 0x20000, CRC(96b844d6) SHA1(981194c249a8fc2534e24ef672380d751a5dc5fd))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(bop_l5)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("tmbopl_5.rom", 0x00000, 0x20000, CRC(fd5c426d) SHA1(e006f8e39cf382249db0b969cf966fd8deaa344a))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(bop_l4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("tmbopl_4.rom", 0x00000, 0x20000, CRC(eea14ecd) SHA1(afd670bdc3680f12360561a1a5e5854718c099f7))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(bop_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("bop_l3.u6", 0x00000, 0x20000, CRC(cd4d219d) SHA1(4e73dca186867ebee07682deab058a45cee53be1))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

ROM_START(bop_l2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("bop_l2.u6", 0x00000, 0x20000, CRC(17ee1f56) SHA1(bee68ed5680455f23dc33e889acec83cba68b1dc))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("mach_u18.l1", 0x000000, 0x20000, CRC(f3f53896) SHA1(4be5a8a27c5ac4718713c05ff2ddf51658a1be27))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u15.l1", 0x080000, 0x20000, CRC(fb49513b) SHA1(01f5243ff258adce3a28b24859eba3f465444bdf))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("mach_u14.l1", 0x100000, 0x20000, CRC(be2a736a) SHA1(ebf7b26a86d3ffcc35eaa1da8e4f432bd281fe15))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

/*===========
/  Test Fixture Alphanumeric
/============*/
ROM_START(tfa_13)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x020000, "user2", 0)
	ROM_LOAD("u6_l3.rom", 0x00000, 0x020000, CRC(bf4a37b5) SHA1(91b8bba6182e818a34252a4b2a0b86a2a44d9c42))
ROM_END

GAME(1990,	tfa_13,		0,		wpc_an,	wpc_an,	wpc_an,	ROT0,	"Bally",				"WPC Test Fixture: Alphanumeric (1.3)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	dd_p7,		dd_l2,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Bally",				"Dr. Dude (PA-7 WPC)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	dd_p06,		dd_l2,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Bally",				"Dr. Dude (PA-6 WPC)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	fh_l9,		0,		wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse L-9 (SL-2m)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	fh_l9b,		fh_l9,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse L-9 (SL-2m) Bootleg Improved German translation",				GAME_IS_SKELETON_MECHANICAL)
GAME(1996,	fh_905h,	fh_l9,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse 9.05H",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	fh_l3,		fh_l9,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse L-3",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	fh_l4,		fh_l9,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse L-4",				GAME_IS_SKELETON_MECHANICAL)
GAME(1990,	fh_l5,		fh_l9,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"Funhouse L-5",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	hd_l3,		0,		wpc_an,	wpc_an,	wpc_an,	ROT0,	"Bally",				"Harley Davidson (L-3)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	hd_l1,		hd_l3,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Bally",				"Harley Davidson (L-1)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1992,	bop_l7,		0,		wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-7)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	bop_l6,		bop_l7,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-6)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	bop_l5,		bop_l7,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-5)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	bop_l4,		bop_l7,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-4)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	bop_l3,		bop_l7,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-3)",				GAME_IS_SKELETON_MECHANICAL)
GAME(1991,	bop_l2,		bop_l7,	wpc_an,	wpc_an,	wpc_an,	ROT0,	"Williams",				"The Machine: Bride of Pinbot (L-2)",				GAME_IS_SKELETON_MECHANICAL)
