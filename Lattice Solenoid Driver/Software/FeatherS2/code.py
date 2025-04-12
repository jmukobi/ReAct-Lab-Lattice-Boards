import time
import board
import digitalio

print("Starting...")

# Change these to match your actual pin connections
DIR_PIN = board.D5
STEP_PIN = board.D6

# Number of steps for a 1/4 turn in full-step mode (200 steps/rev motor)
STEPS_PER_REV = 200
QUARTER_TURN_STEPS = STEPS_PER_REV // 4

# Initialize pins
dir_pin = digitalio.DigitalInOut(DIR_PIN)
dir_pin.direction = digitalio.Direction.OUTPUT

step_pin = digitalio.DigitalInOut(STEP_PIN)
step_pin.direction = digitalio.Direction.OUTPUT

def step_motor(direction: bool, steps: int, delay: float = 0.002):
    dir_pin.value = direction
    for _ in range(steps):
        step_pin.value = True
        time.sleep(delay)
        step_pin.value = False
        time.sleep(delay)

while True:
    # Quarter turn clockwise
    step_motor(direction=True, steps=QUARTER_TURN_STEPS)
    time.sleep(1)

    # Quarter turn counter-clockwise
    step_motor(direction=False, steps=QUARTER_TURN_STEPS)
    time.sleep(1)
