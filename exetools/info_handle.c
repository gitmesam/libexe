/* 
 * Info handle
 *
 * Copyright (c) 2011, Joachim Metz <jbmetz@users.sourceforge.net>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#include "info_handle.h"
#include "exeinput.h"
#include "exetools_libexe.h"

#define INFO_HANDLE_NOTIFY_STREAM	stdout

/* Initializes the info handle
 * Returns 1 if successful or -1 on error
 */
int info_handle_initialize(
     info_handle_t **info_handle,
     liberror_error_t **error )
{
	static char *function = "info_handle_initialize";

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( *info_handle == NULL )
	{
		*info_handle = memory_allocate_structure(
		                info_handle_t );

		if( *info_handle == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create info handle.",
			 function );

			goto on_error;
		}
		if( memory_set(
		     *info_handle,
		     0,
		     sizeof( info_handle_t ) ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear info handle.",
			 function );

			goto on_error;
		}
		if( libexe_file_initialize(
		     &( ( *info_handle )->input_file ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to initialize input file.",
			 function );

			goto on_error;
		}
		( *info_handle )->notify_stream = INFO_HANDLE_NOTIFY_STREAM;
	}
	return( 1 );

on_error:
	if( *info_handle != NULL )
	{
		memory_free(
		 *info_handle );

		*info_handle = NULL;
	}
	return( -1 );
}

/* Frees the info handle and its elements
 * Returns 1 if successful or -1 on error
 */
int info_handle_free(
     info_handle_t **info_handle,
     liberror_error_t **error )
{
	static char *function = "info_handle_free";
	int result            = 1;

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( *info_handle != NULL )
	{
		if( ( *info_handle )->input_file != NULL )
		{
			if( libexe_file_free(
			     &( ( *info_handle )->input_file ),
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free input file.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *info_handle );

		*info_handle = NULL;
	}
	return( result );
}

/* Signals the info handle to abort
 * Returns 1 if successful or -1 on error
 */
int info_handle_signal_abort(
     info_handle_t *info_handle,
     liberror_error_t **error )
{
	static char *function = "info_handle_signal_abort";

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	info_handle->abort = 1;

	if( info_handle->input_file != NULL )
	{
		if( libexe_file_signal_abort(
		     info_handle->input_file,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to signal input file to abort.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Sets the ascii codepage
 * Returns 1 if successful or -1 on error
 */
int info_handle_set_ascii_codepage(
     info_handle_t *info_handle,
     const libcstring_system_character_t *string,
     liberror_error_t **error )
{
	static char *function = "info_handle_set_ascii_codepage";
	int result            = 0;

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	result = exeinput_determine_ascii_codepage(
	          string,
	          &( info_handle->ascii_codepage ),
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine ASCII codepage.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Opens the input
 * Returns 1 if successful or -1 on error
 */
int info_handle_open_input(
     info_handle_t *info_handle,
     const libcstring_system_character_t *filename,
     liberror_error_t **error )
{
	static char *function = "info_handle_open_input";

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libexe_file_open_wide(
	     info_handle->input_file,
	     filename,
	     LIBEXE_OPEN_READ,
	     error ) != 1 )
#else
	if( libexe_file_open(
	     info_handle->input_file,
	     filename,
	     LIBEXE_OPEN_READ,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open input file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes the input
 * Returns the 0 if succesful or -1 on error
 */
int info_handle_close_input(
     info_handle_t *info_handle,
     liberror_error_t **error )
{
	static char *function = "info_handle_close_input";

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libexe_file_close(
	     info_handle->input_file,
	     error ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close input file.",
		 function );

		return( -1 );
	}
	return( 0 );
}

/* Prints the file information to a stream
 * Returns 1 if successful or -1 on error
 */
int info_handle_file_fprint(
     info_handle_t *info_handle,
     liberror_error_t **error )
{
	uint8_t name[ 9 ];

	libexe_section_t *section = NULL;
	static char *function     = "info_handle_file_fprint";
	int number_of_sections    = 0;
	int section_index         = 0;

	if( info_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libexe_file_get_number_of_sections(
	     info_handle->input_file,
	     &number_of_sections,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sections.",
		 function );

		goto on_error;
	}
	fprintf(
	 info_handle->notify_stream,
	 "MZ, PE/COFF executable (EXE) information:\n" );

	fprintf(
	 info_handle->notify_stream,
	 "\tNumber of sections\t: %d\n",
	 number_of_sections );

	fprintf(
	 info_handle->notify_stream,
	 "\n" );

	fprintf(
	 info_handle->notify_stream,
	 "Sections:\n" );

	for( section_index = 0;
	     section_index < number_of_sections;
	     section_index++ )
	{
		if( libexe_file_get_section(
		     info_handle->input_file,
		     section_index,
		     &section,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve section: %d.",
			 function,
			 section_index );

			goto on_error;
		}
		if( libexe_section_get_name(
		     section,
		     name,
		     9,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve section: %d name.",
			 function,
			 section_index );

			goto on_error;
		}
		fprintf(
		 info_handle->notify_stream,
		 "\tName\t\t\t: %s\n",
		 name );

		if( libexe_section_free(
		     &section,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free section.",
			 function );

			goto on_error;
		}
	}
	fprintf(
	 info_handle->notify_stream,
	 "\n" );

	return( 1 );

on_error:
	if( section != NULL )
	{
		libexe_section_free(
		 &section,
		 NULL );
	}
	return( -1 );
}

