import pathlib
import shutil
import subprocess
import tempfile
import unittest


class MagicBoxAudioMathTests(unittest.TestCase):
    def test_pcm_sign_saturation_and_physical_volume_range(self):
        compiler = shutil.which("c++")
        if compiler is None:
            self.skipTest("A C++ compiler is required for the board PCM test")
        board = pathlib.Path(__file__).resolve().parents[2] / "main/boards/folotoy/magicbox"
        with tempfile.TemporaryDirectory() as directory:
            source = pathlib.Path(directory) / "audio_test.cc"
            binary = pathlib.Path(directory) / "audio_test"
            source.write_text(r'''
#include "magicbox_audio_math.h"
#include <cassert>
#include <limits>
int main() {
    using namespace magicbox;
    static_assert(DecodeMicrophone(1000 * 65536) == 8000);
    static_assert(DecodeMicrophone(-1000 * 65536) == -8000);
    static_assert(DecodeMicrophone(INT32_MAX) == INT16_MAX);
    static_assert(DecodeMicrophone(INT32_MIN) == INT16_MIN);
    static_assert(EncodeSpeaker(INT16_MIN, 32768) == INT32_MIN);
    static_assert(EncodeSpeaker(INT16_MAX, 32768) == 2147418112);
    static_assert(EncodeSpeaker(-1000, 0) == 0);
    static_assert(EncodeSpeaker(-1000, 16384) == -32768000);
    static_assert(KnobLevelQ15(-100) == 1638);
    static_assert(KnobLevelQ15(5000) == 32768);
    int previous = 0;
    for (int raw = 0; raw <= 4095; ++raw) {
        int level = KnobLevelQ15(raw);
        assert(level >= previous && level >= 1638 && level <= 32768);
        assert(EncodeSpeaker(-12345, level) < 0);
        assert(EncodeSpeaker(12345, level) > 0);
        assert(EncodeSpeaker(-12345, level) == -EncodeSpeaker(12345, level));
        previous = level;
    }
}
''', encoding="utf-8")
            subprocess.run([compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror",
                            "-I", str(board), str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
