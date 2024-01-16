"""
Serial tool to modify settings of temp sensor
"""

import click
import serial

COMMANDS = {
    "UART_CMD_RESET": b"\x01",
    "UART_CMD_CONFIG_SET_WIFI": b"\x11",
    "UART_CMD_CONFIG_GET_WIFI_SSID": b"\x12",
}
ERR = {
    "UART_ERR_OK": b"\x00",
    "UART_ERR_FAIL": b"\x01",
    "UART_ERR_DISABLED": b"\x02",
    "UART_ERR_INVALID_CMD": b"\x03",
    "UART_ERR_INVALID_VALUE": b"\x04",
}


def _lookup_err(code):
    return list(ERR.keys())[list(ERR.values()).index(code)]


@click.group()
@click.option("--port", help="Port temp sensor is connected to.")
@click.option("--baud", help="Baud rate to connect at.", default=74880)
@click.pass_context
def cli(ctx, port: str, baud: int):
    ctx.ensure_object(dict)

    ctx.obj["port"] = port
    ctx.obj["baud"] = baud


@cli.command('set-wifi')
@click.option("--ssid", help="SSID to connect to")
@click.pass_context
def set_wifi(ctx, ssid: str):
    buf = bytearray()
    buf.extend(COMMANDS["UART_CMD_CONFIG_SET_WIFI"])
    buf.extend(ssid.encode())
    buf.extend(b"\x00")

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    res = conn.read(1)
    click.echo(_lookup_err(res))


@cli.command()
@click.pass_context
def reset(ctx):
    buf = bytearray()
    buf.extend(COMMANDS["UART_CMD_RESET"])

    conn = serial.Serial(ctx.obj["port"], ctx.obj["baud"])
    conn.write(buf)
    

if __name__ == "__main__":
    cli(obj={})
