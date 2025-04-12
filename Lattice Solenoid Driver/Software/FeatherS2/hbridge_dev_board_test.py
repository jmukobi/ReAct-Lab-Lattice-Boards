import time
import board
import digitalio

# Constant time interval (in seconds) for enable pin activation
T = .1  # Adjust as needed

# Pin definitions
DIRECTION_PIN = board.D13  # Direction control pin
ENABLE_PIN = board.D12      # Enable control pin
SLEEP_PIN = board.D9       # Sleep control pin
DISABLE_PIN = board.D6     # Disable control pin

# Set up direction pin as an output
direction = digitalio.DigitalInOut(DIRECTION_PIN)
direction.direction = digitalio.Direction.OUTPUT

# Set up enable pin as an output
enable = digitalio.DigitalInOut(ENABLE_PIN)
enable.direction = digitalio.Direction.OUTPUT

# Set up sleep pin as an output
sleep = digitalio.DigitalInOut(SLEEP_PIN)
sleep.direction = digitalio.Direction.OUTPUT

# Set up disable pin as an output
disable = digitalio.DigitalInOut(DISABLE_PIN)
disable.direction = digitalio.Direction.OUTPUT

# Ensure all pins start off
direction.value = False
enable.value = False
sleep.value = False
disable.value = True

print("Initialized...")
time.sleep(3)

# Main sequence
while True:

    print("\nFWD")
    # Enable the motor on in fwd
    enable.value = True
    sleep.value = False
    direction.value = False
    disable.value = False

    time.sleep(T)

    print("\nOFF")
    # Disable the motor
    enable.value = False
    sleep.value = False
    direction.value = False
    disable.value = True

    time.sleep(1)

    print("\nREV")
    # Enable the motor on in rev
    enable.value = False
    sleep.value = False
    direction.value = True
    disable.value = False

    time.sleep(T)

    print("\nOFF")
    # Disable the motor
    enable.value = False
    sleep.value = False
    direction.value = False
    disable.value = True
    
    # Delay before repeating (for debugging)
    time.sleep(2)