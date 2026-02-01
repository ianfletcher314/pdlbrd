# PDLBRD - Usage Guide

**Guitar Effects Pedalboard Plugin**

PDLBRD (Pedalboard) is a comprehensive guitar effects suite featuring two compressors, distortion, amp simulation, modulation, and reverb - all in a flexible, reorderable signal chain. Perfect for crafting complete guitar tones from clean to crushing.

---

## Use Cases in Modern Rock Production

### Drum Bus Processing

While PDLBRD is designed for guitar, its compressors and amp sections can add unique character to drums.

**Parallel Drum Crush (Creative):**
- Enable Compressor 1: Threshold -30 dB, Ratio 8:1, Attack 1ms, Release 50ms
- Enable Distortion: Drive 30%, Type: Blues, Tone 50%
- Amp Bypass: On
- Effect Order: Comp1 first
- Blend: 20-30%

### Guitar Bus / Individual Tracks

This is PDLBRD's primary purpose - creating complete guitar tones.

**Clean Rhythm Guitar:**
- Compressor 1: Threshold -20 dB, Ratio 3:1, Attack 20ms, Release 150ms
- Distortion: Bypass
- Amp: Type Clean, Gain 20%, Bass 50%, Mid 60%, Treble 55%, Master 70%
- Modulation: Type Chorus, Rate 40%, Depth 30%, Blend 30%
- Reverb: Type Spring, Decay 40%, Blend 20%

**Crunch Rhythm Guitar:**
- Compressor 1: Bypass (or light compression)
- Distortion: Type TS9, Drive 50%, Tone 55%, Level 60%
- Amp: Type Crunch, Gain 50%, Bass 45%, Mid 55%, Treble 50%, Master 75%
- Modulation: Bypass
- Reverb: Type Spring, Decay 30%, Blend 15%

**Heavy Rhythm Guitar:**
- Compressor 1: Bypass
- Distortion: Type RAT, Drive 65%, Tone 50%, Level 55%
- Amp: Type Modern, Gain 70%, Bass 50%, Mid 45%, Treble 55%, Master 80%
- Modulation: Bypass
- Reverb: Type Plate, Decay 20%, Blend 10%

**Modern Metal Rhythm:**
- Compressor 1: Bypass
- Distortion: Type RAT or Muff, Drive 75%, Tone 55%, Level 50%
- Amp: Type Modern, Gain 80%, Bass 55%, Mid 40%, Treble 60%, Master 85%
- Modulation: Bypass
- Reverb: Bypass or very subtle

**Lead Guitar:**
- Compressor 1: Threshold -15 dB, Ratio 4:1, Attack 15ms, Release 200ms
- Distortion: Type TS9, Drive 55%, Tone 60%, Level 65%
- Amp: Type Lead, Gain 55%, Bass 45%, Mid 60%, Treble 55%, Master 70%
- Modulation: Type Chorus, Rate 30%, Depth 25%, Blend 25%
- Reverb: Type Hall, Decay 50%, Blend 25%
- Compressor 2: Threshold -12 dB, Ratio 2:1 (sustain boost)

**Ambient/Post-Rock Guitar:**
- Compressor 1: Threshold -18 dB, Ratio 3:1, Attack 25ms
- Distortion: Bypass or very light
- Amp: Type Clean or Vintage, Gain 25%
- Modulation: Type Tremolo or Vibrato, Rate 50%, Depth 50%
- Reverb: Type Hall, Decay 80%, Blend 50%

### Bass Guitar

PDLBRD's amp and distortion sections work great for bass tones.

**Clean Bass Tone:**
- Compressor 1: Threshold -18 dB, Ratio 4:1, Attack 15ms, Release 150ms
- Distortion: Bypass
- Amp: Type Clean, Gain 30%, Bass 60%, Mid 50%, Treble 40%, Master 70%
- Effect Order: Comp1 -> Amp

**Driven Rock Bass:**
- Compressor 1: Threshold -20 dB, Ratio 3:1
- Distortion: Type Blues, Drive 40%, Tone 45%, Level 55%
- Amp: Type Crunch, Gain 40%, Bass 55%, Mid 55%, Treble 45%
- Compressor 2: Threshold -15 dB, Ratio 2:1 (level out)

### Vocals

Creative use only - PDLBRD can add lo-fi character to vocals.

**Lo-Fi Vocal Effect:**
- Compressor 1: Heavy compression
- Distortion: Type Blues, Drive 20%, Tone 60%
- Amp: Type Vintage, low gain
- Reverb: Type Plate, Blend 30%
- Mix with clean vocal

### Mix Bus / Mastering

Not recommended - use dedicated mastering tools (MasterBus, Automaster)

---

## Recommended Settings

### Quick Reference by Genre

| Style | Distortion | Amp Type | Amp Gain | Key Mods |
|-------|------------|----------|----------|----------|
| Clean | Bypass | Clean | 20-30% | Chorus/Reverb |
| Crunch | TS9 (50%) | Crunch | 45-55% | Light Spring |
| Classic Rock | TS9/Blues (60%) | Vintage | 55-65% | Phaser/Plate |
| Modern Rock | RAT (60%) | Lead | 60-70% | Minimal |
| Metal | RAT/Muff (70%+) | Modern | 75-85% | None/Minimal |
| Ambient | Bypass/Light | Clean | 25% | Heavy Reverb |
| Lead | TS9 (55%) | Lead | 55% | Chorus/Hall |

### Distortion Type Guide

- **TS9**: Mid-focused overdrive - tightens low end, great for leads and stacking
- **RAT**: Aggressive distortion - thick, harmonically rich, great for metal
- **Blues**: Warm, dynamic overdrive - responds to playing dynamics
- **Fuzz**: Classic fuzz tone - thick sustain, great for leads
- **Muff**: Big, sustaining fuzz - wall of sound, great for heavy riffs

### Amp Type Guide

- **Clean**: Crystal clear, lots of headroom - acoustic simulation, jazz, clean rhythm
- **Crunch**: Edge of breakup - blues, classic rock rhythm
- **Lead**: Saturated but articulate - solos, rock rhythm
- **Modern**: Tight, high-gain - metal, djent, modern rock
- **Vintage**: Warm, saggy - classic rock, blues

### Recommended Effect Order

**Standard Guitar:**
Comp1 -> Distortion -> Amp -> Modulation -> Reverb -> Comp2

**Modern High-Gain:**
Comp1 -> Distortion -> Amp -> Comp2 -> Reverb -> Modulation

**Clean/Ambient:**
Comp1 -> Amp -> Modulation -> Reverb -> Comp2 -> Distortion(bypass)

---

## Signal Flow Tips

### Effect Order Strategies

1. **Compression Before Distortion**: Evens out dynamics before gain stages - more consistent saturation
2. **Compression After Amp**: Controls overall dynamics - more natural feel
3. **Modulation Before Reverb**: Standard placement - cleaner effects
4. **Reverb Before Modulation**: Creates "washed out" ambient textures

### Gain Staging

- Keep individual effect levels balanced
- Use Amp Master to control overall output
- Watch input levels on heavy distortion settings
- Use Compressor 2 Makeup to match bypass level

---

## Combining with Other Plugins

### Complete Guitar Chain
1. **PDLBRD** - complete tone shaping
2. **TapeWarm** - add analog warmth
3. **Bus Glue** (on guitar bus) - glue multiple takes

### For Recording DI
1. **PDLBRD** - amp and effects
2. Keep parallel DI track for re-amping flexibility

### With External Amp Sims
- Bypass PDLBRD's Amp section
- Use Distortion and Compressors as "pedals"
- Route to external amp sim
- Return through PDLBRD's Modulation and Reverb

---

## Quick Start Guide

**Get a great crunch tone in 30 seconds:**

1. Load PDLBRD on your guitar track
2. Enable **Amp**, set Type to **Crunch**
3. Set Amp **Gain** to 50%, **Master** to 70%
4. Set Bass/Mid/Treble to 50/55/50
5. Enable **Distortion**, set Type to **TS9**
6. Set Drive to 50%, Tone to 55%, Level to 60%
7. Enable **Reverb**, set Type to **Spring**
8. Set Decay to 35%, Blend to 20%
9. Play and adjust to taste

**Get a heavy metal tone in 30 seconds:**

1. Load PDLBRD on your guitar track
2. Bypass **Compressor 1** and **Compressor 2**
3. Enable **Distortion**, set Type to **RAT**
4. Set Drive to 70%, Tone to 55%, Level to 55%
5. Enable **Amp**, set Type to **Modern**
6. Set Gain to 75%, Master to 80%
7. Set Bass 55%, Mid 45%, Treble 60%
8. Bypass or set very subtle **Reverb** (Plate, Decay 15%, Blend 10%)
9. Palm mute and adjust low end to taste

**Get an ambient clean tone in 30 seconds:**

1. Load PDLBRD on your guitar track
2. Enable **Amp**, set Type to **Clean**
3. Set Gain to 25%, Master to 65%
4. Enable **Modulation**, set Type to **Chorus**
5. Set Rate 35%, Depth 30%, Blend 35%
6. Enable **Reverb**, set Type to **Hall**
7. Set Decay to 70%, Blend to 45%
8. Enable **Compressor 1**: Threshold -18 dB, Ratio 3:1
9. Play arpeggios and enjoy the wash
