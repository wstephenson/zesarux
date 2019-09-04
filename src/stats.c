/*
    ZEsarUX  ZX Second-Emulator And Released for UniX
    Copyright (C) 2013 Cesar Hernandez Bano

    This file is part of ZEsarUX.

    ZEsarUX is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "cpu.h"
#include "stats.h"
#include "menu.h"
#include "compileoptions.h"
#include "debug.h"



char stats_uuid[128]="";

z80_bit stats_enabled={0};
z80_bit stats_asked={0};

void generate_stats_uuid(void)
{

	//Hay un id anterior. conservarlo
	if (stats_uuid[0]!=0) {
		debug_printf (VERBOSE_DEBUG,"Found previous uuid. Preserve it");
		return;
	}

	struct timeval fecha;

	gettimeofday(&fecha, NULL);


	int secs=fecha.tv_sec;
	int microsecs=fecha.tv_usec;

	//printf ("secs %d microsecs %d\n",secs,microsecs);
	//tv_usec

	//El uuid del usuario consta de los segundos.microsegundos cuando se genera

	sprintf(stats_uuid,"%d.%d",secs,microsecs);
	debug_printf (VERBOSE_INFO,"Generated uuid: %s",stats_uuid);

}

void stats_enable(void)
{
	stats_enabled.v=1;
	generate_stats_uuid();
}

void stats_disable(void)
{
	stats_enabled.v=0;
}


void stats_ask_if_enable(void)
{
	int valor_opcion;

	zxvision_menu_generic_message_setting("Send Statistics","Do you want to send anoymous statistics use? The following information is sent to a server:\n"
										"-Autogenerated UUID\n"
										"-Total minutes use\n"
										"-Operating system\n"
	
										,"Send statistics",&valor_opcion);

	stats_asked.v=1;

	//printf ("Valor opcion: %d\n",valor_opcion);

	if (valor_opcion) stats_enable();
	else stats_disable();

	
}


void send_stats_server(void)
{

	if (stats_enabled.v==0) return;

	//prueba tonta de enviar una conexion http a mi servidor
	int http_code;
	char *mem;

	char *mem_after_headers;
	int total_leidos;
	int retorno;

	

	int minutes=stats_get_current_total_minutes_use();

	char query_url_parameters[1024];
	char query_url_parameters_normalized[1024];

	sprintf (query_url_parameters,"UUID=%s&OS=%s&total_minutes_use=%d",stats_uuid,COMPILATION_SYSTEM,minutes);
	//Normalizar solo la parte de parametros. Si hicieramos toda la url, el "/" del inicio de la url se convertiria a %2f
	util_normalize_query_http(query_url_parameters,query_url_parameters_normalized);


	char query_url[1024];
	sprintf (query_url,"/prueba-con?%s",query_url_parameters_normalized);

	//printf ("query url: %s\n",query_url);

    
	retorno=zsock_http("51.83.33.13",query_url,&http_code,&mem,&total_leidos,&mem_after_headers,1,"");
}

int stats_get_current_total_minutes_use(void)
{
	//El tiempo total de config + el tiempo actual desde que arranca
	int uptime_seconds=timer_get_uptime_seconds();
	return total_minutes_use+uptime_seconds/60;
}