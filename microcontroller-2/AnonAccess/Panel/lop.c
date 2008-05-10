/**
 * labor octet protocol
 * \file lop.c
 * \author Daniel Otte
 * \par License
 *  GPLv3
 * 
 * 
 */
 
 
#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "lop.h"
#include "lcd_tools.h"

static void lop_process_l1(lop_ctx_t* ctx, uint8_t b);
static void lop_process_l2(lop_ctx_t* ctx, uint8_t b);

/******************************************************************************/

static
void lop_error(uint8_t b){
	switch(b){
		case 1: /* message in message */
			break;
		case 2: /* stream start while not idle */
			break;
		case 3: /* stream start while not stream */
			break;
		case 4: /* invalid esc-sequence */
			break;
		case 5: /* message to large */
			break;			
		case 6: /* invalid recive state */
			break;
		case 7: /* stream send while not stream */
			break;
		default:
			break;
	}
	for(;;)
		;
}

/******************************************************************************/

void lop_reset(lop_ctx_t* ctx){
	ctx->rxstate = idle;
	ctx->txstate = idle;
	ctx->escaped = 0;
	ctx->msgretstate = idle;
	ctx->msgidx = 0;
	ctx->msglength = 0;
	if(ctx->msgbuffer){
		free(ctx->msgbuffer);
		ctx->msgbuffer = NULL;
	}
	if(ctx->on_reset){
		ctx->on_reset;
	}
}

/******************************************************************************/

void lop_recieve_byte(lop_ctx_t* ctx, uint8_t b){
	lop_process_l1(ctx, b);
}

/******************************************************************************/
static
void lop_process_l1(lop_ctx_t* ctx, uint8_t b){
	if (b==LOP_RESET_CODE){
		lop_reset(ctx);
		return;
	} 
	if (b==LOP_ESC_CODE){
		ctx->escaped = 1;
		return;
	}
	if(!ctx->escaped){
		lop_process_l2(ctx, b);
		return;
	} else {
		ctx->escaped = 0;
		if((b<=0x04) && (b!=0)){ /* escaped data byte */
			uint8_t t[4]={LOP_RESET_CODE, LOP_ESC_CODE, LOP_XON_CODE, LOP_XOFF_CODE};
			lop_process_l2(ctx, t[b-1]);
			return;
		}
		switch(b){
			case LOP_TYPE_MSG:
				if(ctx->rxstate==message){
					/* invalid transition error */
					lop_error(1);
				}
				ctx->msgretstate = ctx->rxstate;
				ctx->rxstate=message;
				if(ctx->msgbuffer){
					free(ctx->msgbuffer);
					ctx->msgbuffer = NULL;
				}
				ctx->msgidx = 0;
				ctx->msglength = 0;
				break;
			case LOP_TYPE_STREAMSYNC:
				if(ctx->rxstate==message){
					/* invalid transition error */
					lop_error(2);
				}
				if(ctx->on_streamsync)
					ctx->on_streamsync();
				break;
			default:
				/* invalid escape-sequence */
				lop_error(4);
				break;
		}
	}
}

/******************************************************************************/
static
void lop_process_l2(lop_ctx_t* ctx, uint8_t b){
	switch(ctx->rxstate){
		case idle:
			/* stream data */
			if(ctx->on_streamrx)
				ctx->on_streamrx(b);
			break;
		case message:
			switch(ctx->msgidx){
				case 0:
					lcd_gotopos(1,20);
					lcd_writechar('a');
					ctx->msglength = (uint16_t)b<<8;
					break;
				case 1:
					lcd_gotopos(1,20-4);
					lcd_hexdump(&(((uint16_t*)&(ctx->msglength))[0]), 2);
					lcd_writechar('b');
					ctx->msglength += b;
					if(!(ctx->msgbuffer=malloc(ctx->msglength))){
						lcd_gotopos(1,20);
						lcd_writechar('X');
						
						/* message to large error */
						lop_error(5);
					}
					break;
				default:
					lcd_gotopos(1,20-2);
					lcd_hexdump(&(((uint8_t*)&(ctx->msgidx))[0]), 1);
					lcd_writechar('d');
					ctx->msgbuffer[ctx->msgidx-2] = b;
					break;
			}
			if(ctx->msgidx==(uint32_t)ctx->msglength+1){ /* end of message */
				lcd_gotopos(1,20);
				lcd_writechar('e');
				if(ctx->on_msgrx)
					ctx->on_msgrx(ctx->msglength, ctx->msgbuffer);
				if(ctx->msgbuffer)
					free(ctx->msgbuffer);
				ctx->msgbuffer = NULL;
				ctx->rxstate = ctx->msgretstate;
			}
			ctx->msgidx++;
			break;
		default:
			/* invalid recive, should never happen */
			lop_error(6);
			break;
	}
}

/******************************************************************************/
static
void lop_sendbyte(lop_ctx_t* ctx, uint8_t b){
		
	if(!(ctx->sendrawbyte)){
		return;
	}
//	lcd_gotopos(1,3);
//	lcd_writechar('a');
	switch(b){
		case LOP_ESC_CODE:
			ctx->sendrawbyte(LOP_ESC_CODE);
			ctx->sendrawbyte(LOP_ESC_ESC);
			break;
		case LOP_RESET_CODE:
			ctx->sendrawbyte(LOP_ESC_CODE);
			ctx->sendrawbyte(LOP_RESET_ESC);
			break;
		case LOP_XON_CODE:
			ctx->sendrawbyte(LOP_ESC_CODE);
			ctx->sendrawbyte(LOP_XON_ESC);
			break;
		case LOP_XOFF_CODE:
			ctx->sendrawbyte(LOP_ESC_CODE);
			ctx->sendrawbyte(LOP_XOFF_ESC);
			break;
		default:
			ctx->sendrawbyte(b);
			break;
	}
//	lcd_gotopos(1,3);
//	lcd_writechar('x');
}

/******************************************************************************/

void lop_sendmessage(lop_ctx_t * ctx,uint16_t length, uint8_t * msg){
	lcd_gotopos(1,2);
	lcd_writechar('a');
	if(!ctx->sendrawbyte)
		return;
	lcd_gotopos(1,2);
	lcd_writechar('b');
	while(ctx->txstate==message)
		;
	lcd_gotopos(1,2);
	lcd_writechar('c');
	ctx->txstate=message;
	ctx->sendrawbyte(LOP_ESC_CODE);
	ctx->sendrawbyte(LOP_TYPE_MSG);
	lcd_gotopos(1,2);
	lcd_writechar('d');
	lop_sendbyte(ctx, length>>8);
	lop_sendbyte(ctx, length&0x00FF);
	lcd_gotopos(1,2);
	lcd_writechar('e');
	while(length--)
		lop_sendbyte(ctx, *msg++);
	lcd_gotopos(1,2);
	lcd_writechar('f');
	ctx->txstate=idle;
}

/******************************************************************************/

void lop_streamsync(lop_ctx_t * ctx){
	if(!ctx->sendrawbyte)
		return;
	while(ctx->txstate==message)
		;
	ctx->txstate=idle;
	ctx->sendrawbyte(LOP_ESC_CODE);
	ctx->sendrawbyte(LOP_TYPE_STREAMSYNC);
}

/******************************************************************************/

void lop_sendstream(lop_ctx_t * ctx, uint8_t b){
	if(!(ctx->sendrawbyte))
		return;
	if(ctx->txstate!=idle){
		/* invalid send error */
		lop_error(7);	
		return;
	}
	lop_sendbyte(ctx, b);
}

/******************************************************************************/

void lop_sendreset(lop_ctx_t * ctx){
	if(ctx->sendrawbyte)
		ctx->sendrawbyte(LOP_RESET_CODE);
}
