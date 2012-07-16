RollCut is used to cut a file in several chunks, based on a rolling hash.

Intended to divide 'similar' big files in smaller chunks, with a high probability of sharing some equal chunks.

Storing this chunks instead of the big files, can provide better space utilization (by using hard-links), and speed up network backups with tools like rsync.

Usage: rollcut [-d desired_chunks] [-m magic_number] file_name
  -d desired_chunks    An estimation of desired chunks. Will be
                       rounded to a power of 2.
  -m magic_number      The number that the rolling hash will be
                       compared to define a cut point.
  file_name            file to cut.


- How can you use it?

0- Download rollcut binary, gzip and dupemerge / fdupes (windows / linux) or any other hardlink deduplication tool for OS.

1- Use "gzip --rsyncable file" and compress each one of your BIG similar files.

2- Use "rollcut file.gzip" to divide your compressed files in small chunks.

3- Use "dupemerge folder" (in windows) to merge equal files using hard links.

If deduplication doesn't find equal files, use "rollcut -d 1024 file.gzip" to cut your file using smaller chunks and run deduplication again.

4- Now you have your BIG files compressed, and with equal chunks of data being stored just once. If you want, you can send them to a remote location using rsync, or just another tool that knows about hard links.

5- When you want your data again, you just do "copy /b *.rollcut  file.gzip" in windows or "cat *.rollcut  >  file.gzip" in another OS.

- How I am using it?

I have some big databases, with only few changes every day. Each daily backup has to be send over the network to a remote location, but the files are too big for the available bandwidth.

I store the backups of seven days as gzipped deduplicated chunks in a folder. This folder is sent every day over the network to a remote site using rsync.

Once you have send a full day, the chunks of the next day that are hard linked with the previous ones, don't need to be send over the network again.


- How it works?

I tried to find a way of sending two big similar files over the network to a remote site. The first option was to use xdelta, but then you need to send a full file every some days to start creating patching deltas.

Then I thought about cutting the file in small fixed chunks. But the if you insert just one byte at the start, the resulting fixed chunks are all different by just one byte.

The solution came while reading about rsync (http://rsync.samba.org), and a rsync friendly gzip (http://svana.org/kleptog/rgzip.html).

I realized that I could try to cut every file using a similar technique than rgzip: by calculating a rolling hash over a window of some bytes, and cutting the file when the hash is equal to a 'magic' number.

The result is that two similar files, usually are cut by the same places, creating chunks with a high probability of being shared between files.


License: simply a BDS license.

* Copyright (c) FerFebles <ferfebles {at} gmail>
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
