/**
 * Copyright (c) 2010, ZineOS development team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution. Neither the name of the ZineOS nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

/**
 * Simple textmode video out
 */
namespace Video {

	/**
	 * Colors
	 */
	enum Color : uint8_t {
		Black,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Brown,
		LightGray,
		DarkGray,
		LightBlue,
		LightGreen,
		LightCyan,
		LightRed,
		LightMagenta,
		Yellow,
		White
	};

	/**
	 * Initalize
	 */
	void Install();

	/**
	 * Ensures that caret is at the line start.
	 */
	void StartLine();

	/**
	 * Write a character
	 */
	void Write(char);

	/**
	 * Write a string
	 */
	void Write(const char *);

	/**
	 * Clear the screen
	 */
	void Clear();

	/**
	 * Set foreground colour
	 */
	void SetFgColor(Color fg);

	/**
	 * Set background colour
	 */
	void SetBgColor(Color bg);

	/**
	 * Set the colors
	 */
	void SetColors(Color fore, Color back);

	/**
	 * Get colours mask
	 */
	uint16_t GetColors();

	/**
	 * set colors
	 */
	void SetColors(uint16_t mask);

};


#endif // VIDEO_H_INCLUDED
