# PythonAI/bot_model.py
import sys
import json

def decide_action(state):
    px, py = state['player']
    ex, ey = state['enemy']
    bullets = state.get('bullets', [])

    # Dodging logic: if any bullet is near horizontally and heading toward enemy
    for bx, by in bullets:
        if abs(by - ey) < 30 and abs(bx - ex) < 200 and bx < ex:
            return "down" if py < ey else "up"  # move out of the way

    dx = px - ex
    dy = py - ey

    if abs(dx) > abs(dy):
        return "right" if dx > 0 else "left"
    else:
        return "down" if dy > 0 else "up"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("stay")
        exit()
    state = json.loads(sys.argv[1])
    print(decide_action(state))
