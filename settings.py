# SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
# SPDX-License-Identifier: MIT
"""
Serial tool to modify settings of temp sensor
"""

from enum import Enum
import struct

import click
import serial

class Commands(Enum):
    UART_CMD_RESET = b"\x01"
    UART_CMD_CONFIG_SET_WIFI_SSID = b"\x11"
    UART_CMD_CONFIG_GET_WIFI_SSID = b"\x12"
    UART_CMD_CONFIG_SET_WIFI_KEY = b"\x13"
    UART_CMD_CONFIG_CLEAR_WIFI = b"\x14"
    UART_CMD_SENSOR_GET_TEMP = b"\x20"
    UART_CMD_SENSOR_GET_HUMIDITY = b"\x21"
    UART_CMD_SYS_GET_UPTIME = b"\x30"


class Err(Enum):
    UART_ERR_OK = b"\x00"
    UART_ERR_FAIL = b"\x01"
    UART_ERR_DISABLED = b"\x02"
    UART_ERR_INVALID_CMD = b"\x03"
    UART_ERR_INVALID_VALUE = b"\x04"
    UART_ERR_NOT_IMPLEMENTED = b"\x05"


def _read(conn: serial.Serial, size: int):
    """
    _read Read response ignoring log statements
    """

    while (True):
        data = conn.read(1)
        log_values = [b"D", b"I", b"W", b"E"]
        if data in log_values:
            log = (data + conn.read_until(b"\n")).decode()
            print(log, end="")
        else:
            left_to_read = size - 1
            if size != 0:
                data += conn.read(left_to_read)
            return data


@click.group()
@click.option("--port", help="Port temp sensor is connected to.", required=True)
@click.option("--baud", help="Baud rate to connect at.", default=74880)
@click.pass_context
def cli(ctx, port: str, baud: int):
    ctx.ensure_object(dict)

    ctx.obj["port"] = port
    ctx.obj["baud"] = baud


@cli.command("clear-wifi-conf")
@click.pass_context
def clear_wifi_conf(ctx):
    buf = bytearray()
    buf.extend(Commands.UART_CMD_CONFIG_CLEAR_WIFI.value)

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    res = _read(conn, 1)
    click.echo(Err(res).name)


@cli.command("get-temp")
@click.pass_context
def get_temp(ctx):
    buf = bytearray()
    buf.extend(Commands.UART_CMD_SENSOR_GET_TEMP.value)

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    temp = _read(conn, 4)
    click.echo(struct.unpack("f", temp)[0])
    res = _read(conn, 1)
    click.echo(Err(res).name)


@cli.command("get-humidity")
@click.pass_context
def get_humidity(ctx):
    buf = bytearray()
    buf.extend(Commands.UART_CMD_SENSOR_GET_HUMIDITY.value)

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    humidity = _read(conn, 4)
    click.echo(struct.unpack("f", humidity)[0])
    res = _read(conn, 1)
    click.echo(Err(res).name)

@cli.command("get-uptime", help="Get system uptime in seconds")
@click.pass_context
def get_uptime(ctx):
    buf = bytearray()
    buf.extend(Commands.UART_CMD_SYS_GET_UPTIME.value)

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    uptime = _read(conn, 8)
    uptime = struct.unpack("q", uptime)[0]
    click.echo(uptime / (1000*1000))
    res = _read(conn, 1)
    click.echo(Err(res).name)



@cli.command()
@click.pass_context
def reset(ctx):
    buf = bytearray()
    buf.extend(Commands.UART_CMD_RESET.value)

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    

if __name__ == "__main__":
    cli(obj={})
