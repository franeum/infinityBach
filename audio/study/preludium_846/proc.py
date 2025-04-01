#!/usr/bin/env python3

import sys
import mido

if __name__ == "__main__":
    if len(sys.argv) == 2:
        midifile = sys.argv[1]
        mid = mido.MidiFile(midifile)
        notes = []

        for note in mid:
            if note.type == "note_on":
                if note.velocity != 0:
                    notes.append(note.note)

        print(f"length: {len(notes)}")

        print()
        print("#" * 100)
        print()

        for note in notes:
            print(note, end=" ")

        print()
        print()
        print("#" * 100)
        print()
    elif len(sys.argv) > 2:
        for arg in sys.argv[1:]:
            midifile = arg
            mid = mido.MidiFile(midifile)
            notes = []

            for note in mid:
                if note.type == "note_on":
                    if note.velocity != 0:
                        notes.append(note.note)

            print("-" * 100)
            print(arg)
            print()
            print(f"length: {len(notes)}")

            print()
            print("#" * 100)
            print()

            for note in notes:
                print(note, end=" ")

            print()
            print()
            print("#" * 100)
            print()
