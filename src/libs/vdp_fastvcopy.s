;===============================================================================
; Fast DoCopy, by Grauw
; In:  HL = pointer to 15-byte VDP command data
; Out: HL = updated
; Chg: A C
;
.include "vdp.s"

;void fastVCopy(char *bitbltData) __sdcccall(1);
_fastVCopy::
								; HL = Param bitbltData
fastVCopy::
		ld      a,#32
		di
		out     (0x99),a
		ld      a,#(17 + 128)
		out     (0x99),a
		ld      c,#0x9B
		call    waitVDPready
		outi					; 15x OUTI
		outi					; (faster than OTIR)
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		ret
