from PIL import Image, ImageDraw
import os

BASE = r"C:\Users\Pasha\EW\1234-main\assets\sprites"
os.makedirs(BASE, exist_ok=True)

def save(img, name):
    img.save(os.path.join(BASE, name))
    print(f"  Created {name} ({img.size})")

def new_frame():
    return Image.new("RGBA", (64, 64), (0, 0, 0, 0))

# ── PLAYER (hooded wanderer, blue cloak, brown boots) ──────────────
def draw_player(draw, ox=0, oy=0, leg_frame=0):
    # boots
    if leg_frame == 0:
        draw.rectangle([24+ox,50+oy, 28+ox,55+oy], fill=(90,60,30,255))
        draw.rectangle([32+ox,50+oy, 36+ox,55+oy], fill=(90,60,30,255))
    elif leg_frame == 1:
        draw.rectangle([22+ox,50+oy, 26+ox,55+oy], fill=(90,60,30,255))
        draw.rectangle([34+ox,48+oy, 38+ox,53+oy], fill=(90,60,30,255))
    else:
        draw.rectangle([26+ox,48+oy, 30+ox,53+oy], fill=(90,60,30,255))
        draw.rectangle([30+ox,50+oy, 34+ox,55+oy], fill=(90,60,30,255))
    # legs
    draw.rectangle([25+ox,44+oy, 29+oy if False else 29+ox,50+oy], fill=(60,60,80,255))
    draw.rectangle([31+ox,44+oy, 35+ox,50+oy], fill=(60,60,80,255))
    # body (tunic)
    draw.rectangle([22+ox,30+oy, 40+ox,46+oy], fill=(50,80,140,255))
    # belt
    draw.rectangle([22+ox,42+oy, 40+ox,45+oy], fill=(120,80,30,255))
    draw.rectangle([29+ox,42+oy, 33+ox,45+oy], fill=(200,170,50,255))
    # arms
    draw.rectangle([18+ox,32+oy, 22+ox,42+oy], fill=(50,80,140,255))
    draw.rectangle([40+ox,32+oy, 44+ox,42+oy], fill=(50,80,140,255))
    # hands
    draw.rectangle([18+ox,42+oy, 22+ox,46+oy], fill=(200,160,120,255))
    draw.rectangle([40+ox,42+oy, 44+ox,46+oy], fill=(200,160,120,255))
    # head
    draw.rectangle([24+ox,18+oy, 38+ox,32+oy], fill=(200,160,120,255))
    # hood
    draw.rectangle([22+ox,16+oy, 40+ox,26+oy], fill=(40,60,120,255))
    draw.rectangle([22+ox,16+oy, 24+ox,30+oy], fill=(40,60,120,255))
    draw.rectangle([38+ox,16+oy, 40+ox,30+oy], fill=(40,60,120,255))
    draw.rectangle([24+ox,14+oy, 38+ox,18+oy], fill=(40,60,120,255))
    # eyes
    draw.rectangle([27+ox,24+oy, 29+ox,26+oy], fill=(20,20,40,255))
    draw.rectangle([33+ox,24+oy, 35+ox,26+oy], fill=(20,20,40,255))
    # cape
    draw.rectangle([22+ox,30+oy, 24+ox,50+oy], fill=(30,50,110,255))
    draw.rectangle([38+ox,30+oy, 40+ox,50+oy], fill=(30,50,110,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_player(dr); save(img, "player_idle.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_player(dr, leg_frame=1); save(img, "player_walk.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_player(dr)
# sword slash
dr.line([44,28, 58,48], fill=(200,200,220,255), width=3)
dr.line([44,30, 56,50], fill=(150,150,180,200), width=2)
save(img, "player_attack.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_player(dr)
dr.rectangle([28+2,22, 34+2,28], fill=(255,80,80,100))
save(img, "player_hurt.png")

# ── WOLF (gray, quadruped, red eyes) ───────────────────────────────
def draw_wolf(draw, frame=0):
    # body
    draw.rectangle([16,32, 48,44], fill=(110,110,120,255))
    draw.rectangle([14,34, 16,42], fill=(110,110,120,255))
    draw.rectangle([48,34, 50,42], fill=(110,110,120,255))
    # belly
    draw.rectangle([20,42, 44,46], fill=(140,140,150,255))
    # legs
    if frame == 0:
        draw.rectangle([18,44, 22,52], fill=(90,90,100,255))
        draw.rectangle([26,44, 30,52], fill=(90,90,100,255))
        draw.rectangle([36,44, 40,52], fill=(90,90,100,255))
        draw.rectangle([44,44, 48,52], fill=(90,90,100,255))
    else:
        draw.rectangle([16,42, 20,50], fill=(90,90,100,255))
        draw.rectangle([28,44, 32,52], fill=(90,90,100,255))
        draw.rectangle([34,42, 38,50], fill=(90,90,100,255))
        draw.rectangle([46,44, 50,52], fill=(90,90,100,255))
    # paws
    draw.rectangle([18,52, 22,54], fill=(70,70,80,255))
    draw.rectangle([26,52, 30,54], fill=(70,70,80,255))
    draw.rectangle([36,52, 40,54], fill=(70,70,80,255))
    draw.rectangle([44,52, 48,54], fill=(70,70,80,255))
    # head
    draw.rectangle([8,26, 22,38], fill=(120,120,130,255))
    # snout
    draw.rectangle([4,30, 10,36], fill=(100,100,110,255))
    # ears
    draw.rectangle([8,22, 12,28], fill=(130,100,90,255))
    draw.rectangle([16,22, 20,28], fill=(130,100,90,255))
    # inner ears
    draw.rectangle([9,23, 11,27], fill=(180,130,120,255))
    draw.rectangle([17,23, 19,27], fill=(180,130,120,255))
    # eyes
    draw.rectangle([10,30, 12,32], fill=(220,40,40,255))
    draw.rectangle([16,30, 18,32], fill=(220,40,40,255))
    # nose
    draw.rectangle([4,32, 6,34], fill=(40,20,20,255))
    # tail
    draw.rectangle([48,30, 54,34], fill=(110,110,120,255))
    draw.rectangle([52,28, 56,32], fill=(120,120,130,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_wolf(dr, 0); save(img, "wolf_idle.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_wolf(dr, 1); save(img, "wolf_walk.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_wolf(dr, 0)
dr.line([4,28, -2,24], fill=(180,40,40,255), width=2)
dr.rectangle([0,22, 6,28], fill=(200,200,200,255))
dr.rectangle([1,24, 3,26], fill=(200,200,200,255))
dr.rectangle([3,24, 5,26], fill=(200,200,200,255))
save(img, "wolf_attack.png")

# ── BEAR (brown, large, menacing) ──────────────────────────────────
def draw_bear(draw, frame=0):
    # body
    draw.rectangle([12,28, 52,46], fill=(100,70,40,255))
    draw.rectangle([10,32, 12,44], fill=(100,70,40,255))
    draw.rectangle([52,32, 54,44], fill=(100,70,40,255))
    # belly
    draw.rectangle([20,40, 44,48], fill=(130,95,55,255))
    # legs
    if frame == 0:
        draw.rectangle([14,46, 20,54], fill=(80,55,30,255))
        draw.rectangle([26,46, 32,54], fill=(80,55,30,255))
        draw.rectangle([36,46, 42,54], fill=(80,55,30,255))
        draw.rectangle([46,46, 52,54], fill=(80,55,30,255))
    else:
        draw.rectangle([12,44, 18,52], fill=(80,55,30,255))
        draw.rectangle([28,46, 34,54], fill=(80,55,30,255))
        draw.rectangle([34,44, 40,52], fill=(80,55,30,255))
        draw.rectangle([48,46, 54,54], fill=(80,55,30,255))
    # paws
    draw.rectangle([14,54, 20,56], fill=(60,40,20,255))
    draw.rectangle([26,54, 32,56], fill=(60,40,20,255))
    draw.rectangle([36,54, 42,56], fill=(60,40,20,255))
    draw.rectangle([46,54, 52,56], fill=(60,40,20,255))
    # head
    draw.rectangle([16,16, 46,32], fill=(110,78,45,255))
    # ears
    draw.rectangle([16,14, 22,20], fill=(90,60,35,255))
    draw.rectangle([40,14, 46,20], fill=(90,60,35,255))
    draw.rectangle([18,15, 20,18], fill=(160,110,70,255))
    draw.rectangle([42,15, 44,18], fill=(160,110,70,255))
    # snout
    draw.rectangle([26,26, 36,34], fill=(130,95,55,255))
    # eyes
    draw.rectangle([22,22, 25,25], fill=(20,10,10,255))
    draw.rectangle([37,22, 40,25], fill=(20,10,10,255))
    # nose
    draw.rectangle([29,28, 33,31], fill=(30,15,15,255))
    # mouth
    draw.rectangle([29,31, 33,33], fill=(80,30,30,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_bear(dr, 0); save(img, "bear_idle.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_bear(dr, 1); save(img, "bear_walk.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_bear(dr, 0)
# claws
dr.rectangle([4,28, 8,32], fill=(200,200,200,255))
dr.rectangle([2,30, 6,34], fill=(200,200,200,255))
dr.rectangle([6,26, 10,30], fill=(200,200,200,255))
save(img, "bear_attack.png")

# ── SKELETON (white bones, dark eye sockets) ───────────────────────
def draw_skeleton(draw, frame=0):
    # ribcage
    for i in range(4):
        draw.rectangle([24,32+i*3, 40,34+i*3], fill=(220,220,210,255))
    # spine
    draw.rectangle([30,28, 34,46], fill=(210,210,200,255))
    # pelvis
    draw.rectangle([24,44, 40,48], fill=(210,210,200,255))
    # legs
    if frame == 0:
        draw.rectangle([24,48, 28,56], fill=(200,200,190,255))
        draw.rectangle([36,48, 40,56], fill=(200,200,190,255))
    else:
        draw.rectangle([22,46, 26,54], fill=(200,200,190,255))
        draw.rectangle([38,48, 42,56], fill=(200,200,190,255))
    # feet
    draw.rectangle([22,56, 28,58], fill=(190,190,180,255))
    draw.rectangle([36,56, 42,58], fill=(190,190,180,255))
    # arms
    draw.rectangle([18,30, 24,44], fill=(200,200,190,255))
    draw.rectangle([40,30, 46,44], fill=(200,200,190,255))
    # hands
    draw.rectangle([18,44, 22,48], fill=(200,200,190,255))
    draw.rectangle([42,44, 46,48], fill=(200,200,190,255))
    # skull
    draw.rectangle([22,14, 42,30], fill=(230,230,220,255))
    # jaw
    draw.rectangle([24,26, 40,30], fill=(210,210,200,255))
    # eye sockets
    draw.rectangle([25,18, 30,24], fill=(20,20,30,255))
    draw.rectangle([34,18, 39,24], fill=(20,20,30,255))
    # glowing eyes
    draw.rectangle([27,20, 28,22], fill=(200,50,50,255))
    draw.rectangle([36,20, 37,22], fill=(200,50,50,255))
    # nose hole
    draw.rectangle([31,23, 33,25], fill=(30,30,40,255))
    # teeth
    draw.rectangle([26,28, 28,30], fill=(240,240,230,255))
    draw.rectangle([30,28, 32,30], fill=(240,240,230,255))
    draw.rectangle([34,28, 36,30], fill=(240,240,230,255))
    draw.rectangle([38,28, 40,30], fill=(240,240,230,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_skeleton(dr, 0); save(img, "skeleton_idle.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_skeleton(dr, 1); save(img, "skeleton_walk.png")
img = new_frame(); dr = ImageDraw.Draw(img); draw_skeleton(dr, 0)
dr.line([46,30, 58,48], fill=(200,200,190,255), width=3)
dr.rectangle([54,44, 60,52], fill=(180,30,30,255))
save(img, "skeleton_attack.png")

# ── NPC MERCHANT (purple robes, friendly) ──────────────────────────
def draw_npc_merchant(draw):
    draw.rectangle([24,50, 28,56], fill=(80,50,30,255))
    draw.rectangle([34,50, 38,56], fill=(80,50,30,255))
    draw.rectangle([22,34, 40,52], fill=(100,50,120,255))
    draw.rectangle([22,48, 40,52], fill=(80,40,100,255))
    draw.rectangle([18,36, 22,48], fill=(100,50,120,255))
    draw.rectangle([40,36, 44,48], fill=(100,50,120,255))
    draw.rectangle([18,48, 22,52], fill=(200,160,120,255))
    draw.rectangle([40,48, 44,52], fill=(200,160,120,255))
    draw.rectangle([24,20, 38,36], fill=(200,160,120,255))
    draw.rectangle([22,16, 40,24], fill=(100,50,120,255))
    draw.rectangle([24,14, 38,18], fill=(100,50,120,255))
    draw.rectangle([28,24, 30,26], fill=(30,20,10,255))
    draw.rectangle([34,24, 36,26], fill=(30,20,10,255))
    draw.rectangle([30,28, 34,30], fill=(180,120,100,255))
    # coin symbol on chest
    draw.rectangle([30,40, 34,44], fill=(220,180,50,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_npc_merchant(dr); save(img, "npc_merchant.png")

# ── NPC QUEST GIVER (brown robes, scroll) ──────────────────────────
def draw_npc_quest(draw):
    draw.rectangle([24,50, 28,56], fill=(80,60,30,255))
    draw.rectangle([34,50, 38,56], fill=(80,60,30,255))
    draw.rectangle([22,34, 40,52], fill=(120,90,50,255))
    draw.rectangle([22,48, 40,52], fill=(100,75,40,255))
    draw.rectangle([18,36, 22,48], fill=(120,90,50,255))
    draw.rectangle([40,36, 44,48], fill=(120,90,50,255))
    draw.rectangle([18,48, 22,52], fill=(200,160,120,255))
    draw.rectangle([40,48, 44,52], fill=(200,160,120,255))
    draw.rectangle([24,20, 38,36], fill=(200,160,120,255))
    draw.rectangle([22,16, 40,24], fill=(120,90,50,255))
    draw.rectangle([24,14, 38,18], fill=(120,90,50,255))
    draw.rectangle([28,24, 30,26], fill=(30,20,10,255))
    draw.rectangle([34,24, 36,26], fill=(30,20,10,255))
    draw.rectangle([30,28, 34,30], fill=(180,120,100,255))
    # scroll in hand
    draw.rectangle([44,38, 50,48], fill=(230,220,180,255))
    draw.rectangle([45,39, 49,47], fill=(200,190,150,255))

img = new_frame(); dr = ImageDraw.Draw(img); draw_npc_quest(dr); save(img, "npc_quest_giver.png")

# ── TILES ──────────────────────────────────────────────────────────
def make_tile(base_color, detail_color, name):
    img = new_frame()
    dr = ImageDraw.Draw(img)
    dr.rectangle([0,0,63,63], fill=base_color)
    # add some texture/detail
    for x in range(0, 64, 8):
        for y in range(0, 64, 8):
            if (x + y) % 16 == 0:
                dr.rectangle([x,y,x+3,y+3], fill=detail_color)
    save(img, name)

make_tile((90,170,60,255), (70,140,45,255), "tile_grass.png")
make_tile((40,100,200,255), (30,80,170,255), "tile_water.png")
make_tile((140,140,130,255), (120,120,110,255), "tile_stone.png")
make_tile((150,120,60,255), (130,100,50,255), "tile_dirt.png")

# tree tile
img = new_frame(); dr = ImageDraw.Draw(img)
dr.rectangle([0,0,63,63], fill=(90,170,60,255))
# trunk
dr.rectangle([28,36,36,56], fill=(100,70,30,255))
# foliage
dr.rectangle([16,12,48,40], fill=(30,100,30,255))
dr.rectangle([20,8,44,36], fill=(40,120,40,255))
dr.rectangle([24,4,40,28], fill=(50,130,50,255))
save(img, "tile_tree.png")

print("\nAll sprites generated!")
