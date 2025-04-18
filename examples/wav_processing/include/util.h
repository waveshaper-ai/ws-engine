#pragma once
#include <climits>
/* Utility functions */

float clamp (float value, float minValue, float maxValue)
{
    value = std::min (value, maxValue);
    value = std::max (value, minValue);
    return value;
}

bool float_equals(float a, float b, float epsilon = 0.000001)
{
        return std::abs(a - b) < epsilon;
}

/* Some utility functions to convert an array of bytes to a float */

float convert24BitsToFloat(unsigned char *b) {

        /* Takes 3 bytes from the data stream and reverses them to convert to signed int (4 bytes).
         *
         * Since we only used 3 bytes, the last byte of the int is 0, so to get a float value scaled to
         * [-1,1] we divide the int by MAX_INT-256 (signed MAX_INT), and that operation evaluates to a float,
         * which we return to the caller. */

        int int32 = b[2] << 24 | b[1] << 16 | b[0] << 8;
        return int32 / static_cast<float>(INT_MAX - 256);                /* scale to [-1,1] */
}

void convertFloatTo24Bits(unsigned char *buf, float *f) {

        int32_t reint = static_cast<uint8_t>(*(f) * 8388608.F);

        buf[2] = static_cast<uint8_t>(reint >> 16) & 0xFF;
        buf[1] = static_cast<uint8_t>(reint >>  8) & 0xFF;
        buf[0] = static_cast<uint8_t>(reint & 0xFF);

}


float convert16BitsToFloat(unsigned char *b) {
        /* Takes 2 bytes from the data stream and combines them to a signed int, and then casts to a float */
        /* Similar to the above function but slightly more compact syntax. */

        //short = (b[0] & 0x00ff) | ((unsigned char)b[1] << 8);

        int16_t int16 =  b[1] << 8 | b[0];
        return static_cast<float> (int16) / static_cast<float> (32768.); /* MAX_SHORT - no need to account for extra 0 */
}

/* Function to output a clipping event
*/

void report_artifact(uint32_t channel, uint32_t, uint32_t sample_rate, float sample, float adjacent_sample, uint32_t sample_pos, const char *type) {
	std::cout << "{\"m\":\"report\",\"artifact\":\"" << type \
                << "\",\"channel\":\"" << channel \
                << "\",\"time\":\"" << (sample_pos / sample_rate) \
                << ",\"sample\":\"" << sample \
                << "\",\"last_sample\":\""<< adjacent_sample \
                << "\",\"sample_pos\":\"" << sample_pos \
                << "\"}" << std::endl;
}


