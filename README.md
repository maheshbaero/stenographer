# stenographer

This code encodes a message from a .txt of upto 1024 characters into a image file.
It encodes that text data into LSBs of the image file, so the image dosent change by a lot.

A password is set to encode the text, and the same password is needed to decode it as well.
A magic sting "Michael" is also encoded as to know that this image was encoded using this programme

usage:
Compile the programme using gcc *c -o <binary_name>
For encoding;
./<binary_name> E <image_file> <text_file> <password>
For decoding
./<binary_name> D <image_file> <output_text_file> <password>

If the password does not match with the encoded one, the message wont be decoded.
