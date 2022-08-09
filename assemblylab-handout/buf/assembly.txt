
target:     file format elf64-x86-64


Disassembly of section .init:

0000000000401000 <_init>:
  401000:	f3 0f 1e fa          	endbr64 
  401004:	48 83 ec 08          	sub    $0x8,%rsp
  401008:	48 8b 05 71 24 00 00 	mov    0x2471(%rip),%rax        # 403480 <__gmon_start__>
  40100f:	48 85 c0             	test   %rax,%rax
  401012:	74 02                	je     401016 <_init+0x16>
  401014:	ff d0                	callq  *%rax
  401016:	48 83 c4 08          	add    $0x8,%rsp
  40101a:	c3                   	retq   

Disassembly of section .plt:

0000000000401020 <.plt>:
  401020:	ff 35 6a 24 00 00    	pushq  0x246a(%rip)        # 403490 <_GLOBAL_OFFSET_TABLE_+0x8>
  401026:	f2 ff 25 6b 24 00 00 	bnd jmpq *0x246b(%rip)        # 403498 <_GLOBAL_OFFSET_TABLE_+0x10>
  40102d:	0f 1f 00             	nopl   (%rax)
  401030:	f3 0f 1e fa          	endbr64 
  401034:	68 00 00 00 00       	pushq  $0x0
  401039:	f2 e9 e1 ff ff ff    	bnd jmpq 401020 <.plt>
  40103f:	90                   	nop
  401040:	f3 0f 1e fa          	endbr64 
  401044:	68 01 00 00 00       	pushq  $0x1
  401049:	f2 e9 d1 ff ff ff    	bnd jmpq 401020 <.plt>
  40104f:	90                   	nop
  401050:	f3 0f 1e fa          	endbr64 
  401054:	68 02 00 00 00       	pushq  $0x2
  401059:	f2 e9 c1 ff ff ff    	bnd jmpq 401020 <.plt>
  40105f:	90                   	nop
  401060:	f3 0f 1e fa          	endbr64 
  401064:	68 03 00 00 00       	pushq  $0x3
  401069:	f2 e9 b1 ff ff ff    	bnd jmpq 401020 <.plt>
  40106f:	90                   	nop
  401070:	f3 0f 1e fa          	endbr64 
  401074:	68 04 00 00 00       	pushq  $0x4
  401079:	f2 e9 a1 ff ff ff    	bnd jmpq 401020 <.plt>
  40107f:	90                   	nop
  401080:	f3 0f 1e fa          	endbr64 
  401084:	68 05 00 00 00       	pushq  $0x5
  401089:	f2 e9 91 ff ff ff    	bnd jmpq 401020 <.plt>
  40108f:	90                   	nop
  401090:	f3 0f 1e fa          	endbr64 
  401094:	68 06 00 00 00       	pushq  $0x6
  401099:	f2 e9 81 ff ff ff    	bnd jmpq 401020 <.plt>
  40109f:	90                   	nop

Disassembly of section .plt.sec:

00000000004010a0 <puts@plt>:
  4010a0:	f3 0f 1e fa          	endbr64 
  4010a4:	f2 ff 25 f5 23 00 00 	bnd jmpq *0x23f5(%rip)        # 4034a0 <puts@GLIBC_2.2.5>
  4010ab:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010b0 <mmap@plt>:
  4010b0:	f3 0f 1e fa          	endbr64 
  4010b4:	f2 ff 25 ed 23 00 00 	bnd jmpq *0x23ed(%rip)        # 4034a8 <mmap@GLIBC_2.2.5>
  4010bb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010c0 <printf@plt>:
  4010c0:	f3 0f 1e fa          	endbr64 
  4010c4:	f2 ff 25 e5 23 00 00 	bnd jmpq *0x23e5(%rip)        # 4034b0 <printf@GLIBC_2.2.5>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010d0 <fprintf@plt>:
  4010d0:	f3 0f 1e fa          	endbr64 
  4010d4:	f2 ff 25 dd 23 00 00 	bnd jmpq *0x23dd(%rip)        # 4034b8 <fprintf@GLIBC_2.2.5>
  4010db:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010e0 <gets@plt>:
  4010e0:	f3 0f 1e fa          	endbr64 
  4010e4:	f2 ff 25 d5 23 00 00 	bnd jmpq *0x23d5(%rip)        # 4034c0 <gets@GLIBC_2.2.5>
  4010eb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010f0 <munmap@plt>:
  4010f0:	f3 0f 1e fa          	endbr64 
  4010f4:	f2 ff 25 cd 23 00 00 	bnd jmpq *0x23cd(%rip)        # 4034c8 <munmap@GLIBC_2.2.5>
  4010fb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

0000000000401100 <exit@plt>:
  401100:	f3 0f 1e fa          	endbr64 
  401104:	f2 ff 25 c5 23 00 00 	bnd jmpq *0x23c5(%rip)        # 4034d0 <exit@GLIBC_2.2.5>
  40110b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

Disassembly of section .text:

0000000000401110 <_start>:
  401110:	f3 0f 1e fa          	endbr64 
  401114:	31 ed                	xor    %ebp,%ebp
  401116:	49 89 d1             	mov    %rdx,%r9
  401119:	5e                   	pop    %rsi
  40111a:	48 89 e2             	mov    %rsp,%rdx
  40111d:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  401121:	50                   	push   %rax
  401122:	54                   	push   %rsp
  401123:	49 c7 c0 f0 13 40 00 	mov    $0x4013f0,%r8
  40112a:	48 c7 c1 80 13 40 00 	mov    $0x401380,%rcx
  401131:	48 c7 c7 9b 12 40 00 	mov    $0x40129b,%rdi
  401138:	ff 15 3a 23 00 00    	callq  *0x233a(%rip)        # 403478 <__libc_start_main@GLIBC_2.2.5>
  40113e:	f4                   	hlt    
  40113f:	90                   	nop

0000000000401140 <_dl_relocate_static_pie>:
  401140:	f3 0f 1e fa          	endbr64 
  401144:	c3                   	retq   
  401145:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40114c:	00 00 00 
  40114f:	90                   	nop

0000000000401150 <deregister_tm_clones>:
  401150:	b8 f0 34 40 00       	mov    $0x4034f0,%eax
  401155:	48 3d f0 34 40 00    	cmp    $0x4034f0,%rax
  40115b:	74 13                	je     401170 <deregister_tm_clones+0x20>
  40115d:	b8 00 00 00 00       	mov    $0x0,%eax
  401162:	48 85 c0             	test   %rax,%rax
  401165:	74 09                	je     401170 <deregister_tm_clones+0x20>
  401167:	bf f0 34 40 00       	mov    $0x4034f0,%edi
  40116c:	ff e0                	jmpq   *%rax
  40116e:	66 90                	xchg   %ax,%ax
  401170:	c3                   	retq   
  401171:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  401178:	00 00 00 00 
  40117c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000401180 <register_tm_clones>:
  401180:	be f0 34 40 00       	mov    $0x4034f0,%esi
  401185:	48 81 ee f0 34 40 00 	sub    $0x4034f0,%rsi
  40118c:	48 89 f0             	mov    %rsi,%rax
  40118f:	48 c1 ee 3f          	shr    $0x3f,%rsi
  401193:	48 c1 f8 03          	sar    $0x3,%rax
  401197:	48 01 c6             	add    %rax,%rsi
  40119a:	48 d1 fe             	sar    %rsi
  40119d:	74 11                	je     4011b0 <register_tm_clones+0x30>
  40119f:	b8 00 00 00 00       	mov    $0x0,%eax
  4011a4:	48 85 c0             	test   %rax,%rax
  4011a7:	74 07                	je     4011b0 <register_tm_clones+0x30>
  4011a9:	bf f0 34 40 00       	mov    $0x4034f0,%edi
  4011ae:	ff e0                	jmpq   *%rax
  4011b0:	c3                   	retq   
  4011b1:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  4011b8:	00 00 00 00 
  4011bc:	0f 1f 40 00          	nopl   0x0(%rax)

00000000004011c0 <__do_global_dtors_aux>:
  4011c0:	f3 0f 1e fa          	endbr64 
  4011c4:	80 3d 3d 23 10 00 00 	cmpb   $0x0,0x10233d(%rip)        # 503508 <completed.8060>
  4011cb:	75 13                	jne    4011e0 <__do_global_dtors_aux+0x20>
  4011cd:	55                   	push   %rbp
  4011ce:	48 89 e5             	mov    %rsp,%rbp
  4011d1:	e8 7a ff ff ff       	callq  401150 <deregister_tm_clones>
  4011d6:	c6 05 2b 23 10 00 01 	movb   $0x1,0x10232b(%rip)        # 503508 <completed.8060>
  4011dd:	5d                   	pop    %rbp
  4011de:	c3                   	retq   
  4011df:	90                   	nop
  4011e0:	c3                   	retq   
  4011e1:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  4011e8:	00 00 00 00 
  4011ec:	0f 1f 40 00          	nopl   0x0(%rax)

00000000004011f0 <frame_dummy>:
  4011f0:	f3 0f 1e fa          	endbr64 
  4011f4:	eb 8a                	jmp    401180 <register_tm_clones>

00000000004011f6 <hack>:
  4011f6:	f3 0f 1e fa          	endbr64 
  4011fa:	55                   	push   %rbp
  4011fb:	48 89 e5             	mov    %rsp,%rbp
  4011fe:	48 83 ec 10          	sub    $0x10,%rsp
  401202:	89 7d fc             	mov    %edi,-0x4(%rbp)
  401205:	bf 08 20 40 00       	mov    $0x402008,%edi
  40120a:	e8 91 fe ff ff       	callq  4010a0 <puts@plt>
  40120f:	81 7d fc a9 db 06 00 	cmpl   $0x6dba9,-0x4(%rbp)
  401216:	75 16                	jne    40122e <hack+0x38>
  401218:	8b 45 fc             	mov    -0x4(%rbp),%eax
  40121b:	89 c6                	mov    %eax,%esi
  40121d:	bf 20 20 40 00       	mov    $0x402020,%edi
  401222:	b8 00 00 00 00       	mov    $0x0,%eax
  401227:	e8 94 fe ff ff       	callq  4010c0 <printf@plt>
  40122c:	eb 14                	jmp    401242 <hack+0x4c>
  40122e:	8b 45 fc             	mov    -0x4(%rbp),%eax
  401231:	89 c6                	mov    %eax,%esi
  401233:	bf 50 20 40 00       	mov    $0x402050,%edi
  401238:	b8 00 00 00 00       	mov    $0x0,%eax
  40123d:	e8 7e fe ff ff       	callq  4010c0 <printf@plt>
  401242:	bf 00 00 00 00       	mov    $0x0,%edi
  401247:	e8 b4 fe ff ff       	callq  401100 <exit@plt>

000000000040124c <read_my_number>:
  40124c:	f3 0f 1e fa          	endbr64 
  401250:	55                   	push   %rbp
  401251:	48 89 e5             	mov    %rsp,%rbp
  401254:	48 83 ec 30          	sub    $0x30,%rsp
  401258:	c7 45 fc 48 00 00 00 	movl   $0x48,-0x4(%rbp)
  40125f:	8b 45 fc             	mov    -0x4(%rbp),%eax
  401262:	89 c6                	mov    %eax,%esi
  401264:	bf 88 20 40 00       	mov    $0x402088,%edi
  401269:	b8 00 00 00 00       	mov    $0x0,%eax
  40126e:	e8 4d fe ff ff       	callq  4010c0 <printf@plt>
  401273:	48 8d 45 d0          	lea    -0x30(%rbp),%rax
  401277:	48 89 c7             	mov    %rax,%rdi
  40127a:	b8 00 00 00 00       	mov    $0x0,%eax
  40127f:	e8 5c fe ff ff       	callq  4010e0 <gets@plt>
  401284:	8b 45 fc             	mov    -0x4(%rbp),%eax
  401287:	89 c6                	mov    %eax,%esi
  401289:	bf 88 20 40 00       	mov    $0x402088,%edi
  40128e:	b8 00 00 00 00       	mov    $0x0,%eax
  401293:	e8 28 fe ff ff       	callq  4010c0 <printf@plt>
  401298:	90                   	nop
  401299:	c9                   	leaveq 
  40129a:	c3                   	retq   

000000000040129b <main>:
  40129b:	f3 0f 1e fa          	endbr64 
  40129f:	55                   	push   %rbp
  4012a0:	48 89 e5             	mov    %rsp,%rbp
  4012a3:	48 83 ec 20          	sub    $0x20,%rsp
  4012a7:	89 7d ec             	mov    %edi,-0x14(%rbp)
  4012aa:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
  4012ae:	41 b9 00 00 00 00    	mov    $0x0,%r9d
  4012b4:	41 b8 00 00 00 00    	mov    $0x0,%r8d
  4012ba:	b9 32 01 00 00       	mov    $0x132,%ecx
  4012bf:	ba 07 00 00 00       	mov    $0x7,%edx
  4012c4:	be 00 00 10 00       	mov    $0x100000,%esi
  4012c9:	bf 00 60 58 55       	mov    $0x55586000,%edi
  4012ce:	e8 dd fd ff ff       	callq  4010b0 <mmap@plt>
  4012d3:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
  4012d7:	48 81 7d f8 00 60 58 	cmpq   $0x55586000,-0x8(%rbp)
  4012de:	55 
  4012df:	74 39                	je     40131a <main+0x7f>
  4012e1:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  4012e5:	be 00 00 10 00       	mov    $0x100000,%esi
  4012ea:	48 89 c7             	mov    %rax,%rdi
  4012ed:	e8 fe fd ff ff       	callq  4010f0 <munmap@plt>
  4012f2:	48 8b 05 07 22 10 00 	mov    0x102207(%rip),%rax        # 503500 <stderr@@GLIBC_2.2.5>
  4012f9:	ba 00 60 58 55       	mov    $0x55586000,%edx
  4012fe:	be b8 20 40 00       	mov    $0x4020b8,%esi
  401303:	48 89 c7             	mov    %rax,%rdi
  401306:	b8 00 00 00 00       	mov    $0x0,%eax
  40130b:	e8 c0 fd ff ff       	callq  4010d0 <fprintf@plt>
  401310:	bf 01 00 00 00       	mov    $0x1,%edi
  401315:	e8 e6 fd ff ff       	callq  401100 <exit@plt>
  40131a:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  40131e:	48 05 f8 ff 0f 00    	add    $0xffff8,%rax
  401324:	48 89 05 ed 21 10 00 	mov    %rax,0x1021ed(%rip)        # 503518 <stack_top>
  40132b:	48 8b 15 e6 21 10 00 	mov    0x1021e6(%rip),%rdx        # 503518 <stack_top>
  401332:	48 89 e0             	mov    %rsp,%rax
  401335:	48 89 d4             	mov    %rdx,%rsp
  401338:	48 89 c2             	mov    %rax,%rdx
  40133b:	48 89 15 ce 21 10 00 	mov    %rdx,0x1021ce(%rip)        # 503510 <global_save_stack>
  401342:	b8 00 00 00 00       	mov    $0x0,%eax
  401347:	e8 00 ff ff ff       	callq  40124c <read_my_number>
  40134c:	bf e0 20 40 00       	mov    $0x4020e0,%edi
  401351:	e8 4a fd ff ff       	callq  4010a0 <puts@plt>
  401356:	48 8b 05 b3 21 10 00 	mov    0x1021b3(%rip),%rax        # 503510 <global_save_stack>
  40135d:	48 89 c4             	mov    %rax,%rsp
  401360:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401364:	be 00 00 10 00       	mov    $0x100000,%esi
  401369:	48 89 c7             	mov    %rax,%rdi
  40136c:	e8 7f fd ff ff       	callq  4010f0 <munmap@plt>
  401371:	b8 00 00 00 00       	mov    $0x0,%eax
  401376:	c9                   	leaveq 
  401377:	c3                   	retq   
  401378:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40137f:	00 

0000000000401380 <__libc_csu_init>:
  401380:	f3 0f 1e fa          	endbr64 
  401384:	41 57                	push   %r15
  401386:	4c 8d 3d 0b 1f 00 00 	lea    0x1f0b(%rip),%r15        # 403298 <__frame_dummy_init_array_entry>
  40138d:	41 56                	push   %r14
  40138f:	49 89 d6             	mov    %rdx,%r14
  401392:	41 55                	push   %r13
  401394:	49 89 f5             	mov    %rsi,%r13
  401397:	41 54                	push   %r12
  401399:	41 89 fc             	mov    %edi,%r12d
  40139c:	55                   	push   %rbp
  40139d:	48 8d 2d fc 1e 00 00 	lea    0x1efc(%rip),%rbp        # 4032a0 <__do_global_dtors_aux_fini_array_entry>
  4013a4:	53                   	push   %rbx
  4013a5:	4c 29 fd             	sub    %r15,%rbp
  4013a8:	48 83 ec 08          	sub    $0x8,%rsp
  4013ac:	e8 4f fc ff ff       	callq  401000 <_init>
  4013b1:	48 c1 fd 03          	sar    $0x3,%rbp
  4013b5:	74 1f                	je     4013d6 <__libc_csu_init+0x56>
  4013b7:	31 db                	xor    %ebx,%ebx
  4013b9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  4013c0:	4c 89 f2             	mov    %r14,%rdx
  4013c3:	4c 89 ee             	mov    %r13,%rsi
  4013c6:	44 89 e7             	mov    %r12d,%edi
  4013c9:	41 ff 14 df          	callq  *(%r15,%rbx,8)
  4013cd:	48 83 c3 01          	add    $0x1,%rbx
  4013d1:	48 39 dd             	cmp    %rbx,%rbp
  4013d4:	75 ea                	jne    4013c0 <__libc_csu_init+0x40>
  4013d6:	48 83 c4 08          	add    $0x8,%rsp
  4013da:	5b                   	pop    %rbx
  4013db:	5d                   	pop    %rbp
  4013dc:	41 5c                	pop    %r12
  4013de:	41 5d                	pop    %r13
  4013e0:	41 5e                	pop    %r14
  4013e2:	41 5f                	pop    %r15
  4013e4:	c3                   	retq   
  4013e5:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
  4013ec:	00 00 00 00 

00000000004013f0 <__libc_csu_fini>:
  4013f0:	f3 0f 1e fa          	endbr64 
  4013f4:	c3                   	retq   

Disassembly of section .fini:

00000000004013f8 <_fini>:
  4013f8:	f3 0f 1e fa          	endbr64 
  4013fc:	48 83 ec 08          	sub    $0x8,%rsp
  401400:	48 83 c4 08          	add    $0x8,%rsp
  401404:	c3                   	retq   
