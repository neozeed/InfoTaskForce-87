/* File: file.c */
/* (C)opyright 1987 InfoTaskforce. */

//#define DO_FILE 0

#ifdef _WIN32
#include	<stdio.h>
FILE	*game_file ;
#endif
#include	"infocom.h"

#define		name_size	10

char	name[name_size + 1] ;


unsigned char _binary_planetfa_dat_start[] = {
#include "planetfa.h"
};


int fseekp;

read_header ( head )
header	*head ;
{
	/*
		Read the Data File Header.

		This routine does not read the data-file header
		directly into a header structure because certain
		machines like the VAX 11/780 store integers in
		a different way to machines based on processors
		like the 68000 ( a 68000 stores the high byte
		first, while a VAX stores the low byte first ).
		Consequently, if the header is read directly
		into a structure, the integer values are
		interpreted differently by the two machines.
	*/
#if DO_FILE
	header	buffer ;

	if ( fseek ( game_file,0L,0 ) < 0 )
	{
		printf ( "Failed to Seek Start of File\n" ) ;
		quit () ;
	}
	else
	{
		if ( fread ((char *)(&buffer),sizeof(header),1,game_file ) != 1 )
		{
			printf ( "Failed to read File Header\n" ) ;
			quit () ;
		}
		else
		{
			assign ( head,&buffer ) ;
			if (( head -> z_code_version != 0x03 ) ||
										( head -> score_or_time & 0x01 ))
				error ( err_header ) ;
		}
	}
#else
	header buffer ;
	fseekp=0;
//printf("memcpy");fflush(stdout);
	memcpy((char *)(&buffer),_binary_planetfa_dat_start,sizeof(header));
	assign ( head,&buffer ) ;
//printf("\n");fflush(stdout);
	if (( head -> z_code_version != 0x03 ) ||( head -> score_or_time & 0x01 ))
		error ( err_header ) ;
	
#endif
}

assign ( head,buffer )
header	*head,*buffer ;
{
	/*
		Process the raw header data in "buffer" and put
		it into the appropriate fields in "head". This
		processing is required because of the way different
		machines internally represent 'words'.
	*/

	byte	*ptr ;
	int		i ;

	ptr = (byte *)buffer ;
	head -> z_code_version	= *ptr++ ;
	head -> score_or_time	= *ptr++ ;
	head -> release			= make_word ( &ptr ) ;
	head -> resident_bytes	= make_word ( &ptr ) ;
	head -> start			= make_word ( &ptr ) ;
	head -> vocab			= make_word ( &ptr ) ;
	head -> object_list		= make_word ( &ptr ) ;
	head -> globals			= make_word ( &ptr ) ;
	head -> save_bytes		= make_word ( &ptr ) ;
	head -> script_status	= make_word ( &ptr ) ;
	for ( i = 0 ; i < 6 ; i++ )
		head -> serial_no[i] = *ptr++ ;
	head -> common_word		= make_word ( &ptr ) ;
	head -> verify_length	= make_word ( &ptr ) ;
	head -> verify_checksum	= make_word ( &ptr ) ;
	for ( i = 0 ; i < 17 ; i++ )
		head -> padding[i] = *ptr++ ;
}

word
make_word ( ptr )
byte	**ptr ;
{
	/*
		Form a word from two bytes.
		(High byte before Low byte)
	*/

	word	value ;

	value = *(*ptr)++ ;
	value = ( value << 8 ) + *(*ptr)++ ;
	return ( value ) ;
}

int
open_file ( filename )
char	*filename ;
{
	/* Open a File for Reading */
fseekp=0;
#if DO_FILE
	game_file = fopen ( filename,"rb" ) ;
	if ( game_file == NULL)	/*(FILE *)0 )*/
		return ( false ) ;
#endif
	return ( true ) ;

}

close_file ()
{
	/* Close an Open File */
#ifdef DO_FILE
	if ( fclose ( game_file ) )
		printf ( "Cannot Close Game File\n" ) ;
#endif
}

load_page ( block,num_blocks,ptr )
word	block ;
word	num_blocks ;
byte	*ptr ;
{
	long	offset ;

	/*
		Read "num_block" blocks from Game File,
		starting with block "block", at the
		location pointed to by "ptr".
	*/
#if DO_FILE
	offset = (long) block * block_size ;
	if ( fseek ( game_file,offset,0 ) < 0 )
	{
		printf ( "Failed to Seek required Blocks\n" ) ;
		quit () ;
	}
	else
		fread ((char *)ptr,block_size,(int)num_blocks,game_file);

#else
	offset = (long) block * block_size ;
	fseekp=offset;
	memcpy(ptr,_binary_planetfa_dat_start+fseekp,num_blocks*block_size);
	fseekp=fseekp+(num_blocks*block_size);
#endif
}

filename ()
{
	char	ch ;
	int		i ;

	echo ( "Filename: " ) ;
	i = 0 ;
	while (( ch = read_char () ) != '\n' )
	{
		/* Handle backspaces */

		if (( ch == back_space ) && ( i != 0 ))
			--i ;

		/* Convert Uppercase to Lowercase */

		if (( ch >= 'A' ) && ( ch <= 'Z' ))
			ch = ch - 'A' + 'a' ;

		/* Handle Alpha-numeric Characters */

		if (( ch >= 'a' ) && ( ch <= 'z' ) && ( i < name_size ))
			name[i++] = ch ;
		if (( ch >= '0' ) && ( ch <= '9' ) && ( i < name_size ))
			name[i++] = ch ;

		/* Handle Pathname Characters */

		if ((ch == '/' || ch == ':' || ch == '.') && (i < name_size))
			name[i++] = ch ;
	}
	name[i] = '\0' ;
	if ( i == 0 )
	{
		echo ( "Bad Filename. Try Again...\n" ) ;
		filename () ;
	}
}

save ()
{
	extern byte		*base_ptr ;
	extern word		save_blocks ;
	int fd;
#if 0	
	FILE	*save_file ;

	/*
		Save a Game.

		It is not necessary to save the stack, the
		stack pointers, or the program counter.
	*/

	filename () ;

	save_file = fopen ( name,"wb" ) ;
	if ( save_file == (FILE *)0 )
	{
		ret_value ( false ) ;
		return ;
	}
	if (fwrite((char *)base_ptr,block_size,(int)save_blocks,save_file) != save_blocks)
	{
		fclose(save_file);
		ret_value ( false ) ;
		return ;
	}
	if ( fclose ( save_file ) )
	{
		ret_value ( false ) ;
		return ;
	}
#else
		ret_value ( false ) ;
		return ;
#endif
}

Boolean
check ( info )
header	*info ;
{
	extern header	data_head ;

	if ( info -> z_code_version != data_head.z_code_version )
		return ( false ) ;
	if ( info -> score_or_time != data_head.score_or_time )
		return ( false ) ;
	if ( info -> release != data_head.release )
		return ( false ) ;
	if ( info -> verify_length != data_head.verify_length )
		return ( false ) ;
	if ( info -> verify_checksum != data_head.verify_checksum )
		return ( false ) ;
	return ( true ) ;
}

restore ()
{
	extern byte		*base_ptr ;
	extern word		save_blocks ;
#if 0
	FILE	*save_file ;
	header	test ;
	Boolean	OK = true ;

	/* Restore a Saved Game */

	filename () ;
	save_file = fopen ( name,"rb" ) ;
	if ( save_file == (FILE *)0 )
	{
		ret_value ( false ) ;
		return ;
	}
	if (fread((char *)base_ptr,block_size,(int)save_blocks,save_file) != save_blocks)
		OK = false;
	if ( fclose ( save_file ) )
		OK = false;
	assign (&test,(header *)base_ptr ) ;
	if ( OK && check( &test ))
		ret_value ( true );
	else
	{
		echo ( "Wrong Game or Version ...\nRestarting ...\n" ) ;
		restart () ;
	}
#else
		ret_value ( false ) ;
		return ;
#endif
}
