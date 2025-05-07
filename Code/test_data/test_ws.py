import asyncio
import websockets
import csv
import datetime

# Replace with your ESP's IP address and port.
WEBSOCKET_URI = "ws://192.168.8.178:81/"

async def listen_and_save():
    # Open the CSV file in append mode.
    # If you want to create a new file every run, use "w" mode.
    with open("C:/Users/Subodha/Desktop/Desk/edp/ThermoBand-EN1190/Code/test_data/rawData/test_output_data_6.csv", "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        # Optionally, write a header if file is empty.
        # writer.writerow(["Timestamp", "Data"])
        try:
            async with websockets.connect(WEBSOCKET_URI) as websocket:
                print("Connected to WebSocket server.")
                while True:
                    try:
                        # Wait for a new message from the server.
                        data = await websocket.recv()
                        timestamp = datetime.datetime.now().isoformat()
                        # Write the timestamp and data as a new row.
                        writer.writerow([timestamp, data])
                        csvfile.flush()  # Flush to disk immediately.
                        print(f"{timestamp} - {data}")
                    except websockets.ConnectionClosed:
                        print("Connection closed. Exiting.")
                        break
        except TimeoutError:    
            print("Connection Timeout. Exiting")
        except KeyboardInterrupt:
            print("Ending logging...")

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(listen_and_save())


