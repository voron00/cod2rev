#include "i_common.h"
#include "i_shared.h"
#include "netchan.h"
#include "sys_net.h"

int ip_socket;
int ipx_socket;

char *NET_ErrorString( void )
{
	int code;

	code = errno;
	return strerror( code );
}

void NetadrToSockadr( netadr_t *a, struct sockaddr_in *s )
{
	memset( s, 0, sizeof( *s ) );

	if ( a->type == NA_BROADCAST )
	{
		s->sin_family = AF_INET;

		s->sin_port = a->port;
		*(int *)&s->sin_addr = -1;
	}
	else if ( a->type == NA_IP )
	{
		s->sin_family = AF_INET;

		*(int *)&s->sin_addr = *(int *)&a->ip;
		s->sin_port = a->port;
	}
}

void SockadrToNetadr( struct sockaddr_in *s, netadr_t *a )
{
	*(int *)&a->ip = *(int *)&s->sin_addr;
	a->port = s->sin_port;
	a->type = NA_IP;
}

qboolean Sys_SendPacket( int length, const void *data, netadr_t to )
{
	int ret;
	struct sockaddr_in addr;
	int net_socket;

	if ( to.type == NA_BROADCAST )
	{
		net_socket = ip_socket;
	}
	else if ( to.type == NA_IP )
	{
		net_socket = ip_socket;
	}
	else if ( to.type == NA_IPX )
	{
		net_socket = ipx_socket;
	}
	else if ( to.type == NA_BROADCAST_IPX )
	{
		net_socket = ipx_socket;
	}
	else
	{
		Com_Error( ERR_FATAL, "NET_SendPacket: bad address type" );
		return qfalse;
	}

	if ( !net_socket )
	{
		return qfalse;
	}

	NetadrToSockadr( &to, &addr );

	ret = sendto( net_socket, data, length, 0, (struct sockaddr *)&addr, sizeof( addr ) );

	if ( ret == -1 )
	{
		Com_Printf( "NET_SendPacket ERROR: %s to %s\n", NET_ErrorString(),
		            NET_AdrToString( to ) );
		return qfalse;
	}

	return qtrue;
}

qboolean    Sys_GetPacket( netadr_t *net_from, msg_t *net_message )
{
	int ret;
	struct sockaddr_in from;
	socklen_t fromlen;
	int net_socket;
	int protocol;
	int err;

	for ( protocol = 0 ; protocol < 2 ; protocol++ )
	{
		if ( protocol == 0 )
		{
			net_socket = ip_socket;
		}
		else
		{
			net_socket = ipx_socket;
		}

		if ( !net_socket )
		{
			continue;
		}

		fromlen = sizeof( from );
		ret = recvfrom( net_socket, net_message->data, net_message->maxsize
		                , 0, (struct sockaddr *)&from, &fromlen );

		SockadrToNetadr( &from, net_from );
		// bk000305: was missing
		net_message->readcount = 0;

		if ( ret == -1 )
		{
			err = errno;

			if ( err == EWOULDBLOCK || err == ECONNREFUSED )
			{
				continue;
			}
			Com_Printf( "NET_GetPacket: %s from %s\n", NET_ErrorString(),
			            NET_AdrToString( *net_from ) );
			continue;
		}

		if ( ret == net_message->maxsize )
		{
			Com_Printf( "Oversize packet from %s\n", NET_AdrToString( *net_from ) );
			continue;
		}

		net_message->cursize = ret;
		return qtrue;
	}

	return qfalse;
}