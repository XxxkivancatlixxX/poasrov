#!/usr/bin/env python3
"""
ROV Telemetry Bridge for Raspberry Pi 5
Reads binary telemetry from Pixhawk serial port and forwards to TCP socket
Also supports reading from stdin (for QEMU testing)

Usage: 
  python3 telemetry_bridge.py              # Real hardware on /dev/ttyACM0:57600
  python3 telemetry_bridge.py [tcp_port]   # Real hardware on custom TCP port
  python3 telemetry_bridge.py --stdin [tcp_port]  # QEMU stdin mode
"""

import socket
import struct
import sys
import threading
import time
from datetime import datetime

# Try to import serial module (optional for QEMU testing)
try:
    import serial
    SERIAL_AVAILABLE = True
except ImportError:
    SERIAL_AVAILABLE = False
    print("[WARN] pyserial not installed - stdin mode only")

# Configuration
SERIAL_PORT = "/dev/ttyACM0"
SERIAL_BAUDRATE = 57600
TCP_PORT = 5760
TCP_HOST = "0.0.0.0"  # Listen on all interfaces

class TelemetryBridge:
    def __init__(self, serial_port=None, baudrate=57600, tcp_port=5760, use_stdin=False):
        self.serial_port = serial_port or SERIAL_PORT
        self.baudrate = baudrate
        self.tcp_port = tcp_port
        self.use_stdin = use_stdin
        self.ser = None
        self.server_socket = None
        self.client_sockets = []
        self.running = True
        self.packet_count = 0
        self.last_status_time = time.time()
        
    def connect_serial(self):
        """Connect to Pixhawk via serial or stdin"""
        if self.use_stdin:
            print(f"[{self._timestamp()}] Using STDIN for input (QEMU mode)")
            print(f"[{self._timestamp()}] Make sure QEMU is outputting telemetry to stdout")
            self.ser = sys.stdin.buffer  # Binary stdin
            return True
        
        if not SERIAL_AVAILABLE:
            print(f"[{self._timestamp()}] ERROR: pyserial not available")
            print(f"        Install with: pip3 install pyserial")
            return False
            
        try:
            self.ser = serial.Serial(
                port=self.serial_port,
                baudrate=self.baudrate,
                timeout=1.0
            )
            print(f"[{self._timestamp()}] Connected to {self.serial_port} at {self.baudrate} baud")
            return True
        except Exception as e:
            print(f"[{self._timestamp()}] ERROR: Cannot open {self.serial_port}: {e}")
            print(f"        Check: ls -la /dev/ttyACM0")
            print(f"        Or: sudo chmod 666 /dev/ttyACM0")
            print(f"        Or use QEMU mode: python3 telemetry_bridge.py --stdin")
            return False
    
    def start_tcp_server(self):
        """Start TCP server for clients"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((TCP_HOST, self.tcp_port))
            self.server_socket.listen(5)
            self.server_socket.settimeout(1.0)
            print(f"[{self._timestamp()}] TCP server listening on {TCP_HOST}:{self.tcp_port}")
        except Exception as e:
            print(f"[{self._timestamp()}] ERROR: Cannot start TCP server: {e}")
            return False
        return True
    
    def accept_clients(self):
        """Accept incoming TCP connections"""
        while self.running:
            try:
                client_socket, addr = self.server_socket.accept()
                print(f"[{self._timestamp()}] Client connected: {addr[0]}:{addr[1]}")
                self.client_sockets.append(client_socket)
            except socket.timeout:
                continue
            except Exception as e:
                if self.running:
                    print(f"[{self._timestamp()}] Accept error: {e}")
    
    def broadcast_telemetry(self, data):
        """Send telemetry to all connected clients"""
        dead_sockets = []
        for client_socket in self.client_sockets:
            try:
                client_socket.sendall(data)
            except Exception as e:
                print(f"[{self._timestamp()}] Client disconnected: {e}")
                dead_sockets.append(client_socket)
        
        for sock in dead_sockets:
            try:
                sock.close()
            except:
                pass
            self.client_sockets.remove(sock)
    
    def read_serial_data(self):
        """Read telemetry from Pixhawk serial port or stdin AND forward control from clients to Pixhawk"""
        buffer = bytearray()
        
        while self.running:
            try:
                if self.use_stdin:
                    # For stdin (QEMU), read byte by byte
                    byte = sys.stdin.buffer.read(1)
                    if byte:
                        buffer.extend(byte)
                    else:
                        # EOF on stdin
                        time.sleep(0.1)
                else:
                    # For serial port
                    if self.ser and self.ser.in_waiting > 0:
                        byte = self.ser.read(1)
                        if byte:
                            buffer.extend(byte)
                    else:
                        time.sleep(0.001)
                
                # Once we have a complete packet, broadcast it
                if len(buffer) >= 512:
                    self.broadcast_telemetry(bytes(buffer))
                    self.packet_count += 1
                    
                    if self.packet_count % 10 == 0:
                        print(f"[{self._timestamp()}] Packets: {self.packet_count} | Clients: {len(self.client_sockets)}")
                    
                    buffer = bytearray()
                    
                # Read control packets from clients and forward to Pixhawk
                self._forward_client_commands()
                    
            except KeyboardInterrupt:
                self.running = False
            except Exception as e:
                if self.use_stdin:
                    print(f"[{self._timestamp()}] Stdin read error: {e}")
                else:
                    print(f"[{self._timestamp()}] Serial read error: {e}")
                time.sleep(1)
    
    def _forward_client_commands(self):
        """Read control commands from connected clients and forward to Pixhawk"""
        dead_sockets = []
        for client_socket in self.client_sockets:
            try:
                # Non-blocking receive of control packets
                client_socket.settimeout(0.001)
                data = client_socket.recv(4096)
                if data:
                    # Forward control packet to Pixhawk
                    if self.ser:
                        self.ser.write(data)
                    elif self.use_stdin:
                        sys.stdout.buffer.write(data)
                        sys.stdout.buffer.flush()
                else:
                    dead_sockets.append(client_socket)
            except socket.timeout:
                # No data available - this is normal for non-blocking
                pass
            except Exception as e:
                print(f"[{self._timestamp()}] Client read error: {e}")
                dead_sockets.append(client_socket)
        
        # Clean up dead connections
        for sock in dead_sockets:
            try:
                sock.close()
            except:
                pass
            if sock in self.client_sockets:
                self.client_sockets.remove(sock)
    
    def run(self):
        """Main loop"""
        if not self.connect_serial():
            return False
        
        if not self.start_tcp_server():
            return False
        
        # Start client accept thread
        accept_thread = threading.Thread(target=self.accept_clients, daemon=True)
        accept_thread.start()
        
        # Start serial read thread
        serial_thread = threading.Thread(target=self.read_serial_data, daemon=True)
        serial_thread.start()
        
        print(f"[{self._timestamp()}] Bridge running...")
        print(f"        Pixhawk: {self.serial_port} @ {self.baudrate} baud")
        print(f"        TCP: {TCP_HOST}:{self.tcp_port}")
        print(f"        Connect GUI to: {self._get_local_ip()}:{self.tcp_port}")
        print("")
        
        try:
            while self.running:
                time.sleep(1)
        except KeyboardInterrupt:
            self.shutdown()
        
        return True
    
    def shutdown(self):
        """Graceful shutdown"""
        print(f"\n[{self._timestamp()}] Shutting down...")
        self.running = False
        
        for sock in self.client_sockets:
            try:
                sock.close()
            except:
                pass
        
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
        
        if self.ser:
            try:
                self.ser.close()
            except:
                pass
        
        print(f"[{self._timestamp()}] Bridge stopped")
    
    @staticmethod
    def _timestamp():
        return datetime.now().strftime("%H:%M:%S")
    
    @staticmethod
    def _get_local_ip():
        """Get local IP address"""
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect(("8.8.8.8", 80))
            ip = s.getsockname()[0]
            s.close()
            return ip
        except:
            return "192.168.1.2"

if __name__ == "__main__":
    use_stdin = False
    tcp_port = TCP_PORT
    serial_port = SERIAL_PORT
    
    # Parse arguments
    for arg in sys.argv[1:]:
        if arg == "--stdin":
            use_stdin = True
        elif arg.startswith("-"):
            continue
        else:
            # Assume it's a TCP port number
            try:
                tcp_port = int(arg)
            except ValueError:
                pass
    
    bridge = TelemetryBridge(serial_port, SERIAL_BAUDRATE, tcp_port, use_stdin=use_stdin)
    
    # Print startup info
    mode = "STDIN (QEMU)" if use_stdin else f"{serial_port}@{SERIAL_BAUDRATE}"
    print(f"\n=== Telemetry Bridge ===")
    print(f"Input:  {mode}")
    print(f"Output: TCP {TCP_HOST}:{tcp_port}")
    print(f"Status: Starting...\n")
    
    try:
        bridge.run()
    except KeyboardInterrupt:
        bridge.shutdown()
