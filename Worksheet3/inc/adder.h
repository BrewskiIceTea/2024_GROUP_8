// b e g i n = adder . h ============
// The f o l l o w i n g two l i n e s p r e v e n t adder . h from b e i n g i n c l u d e d
// more than once i n any s o u r c e ( . cpp ) f i l e . I f t h i s were t o happen
// i t would c a u s e problems i n t h e c o m p i l e p r o c e s s but i t i s d i f f i c u l t t o
// p r e v e n t manually i n l a r g e p r o j e c t s . These #i f n d e f , #d e f i n e and #e n d i f
// commands f o r an ” i n c l u d e guard ” and a r e t y p e s o f c o m p i l e r d i r e c t i v e .
// The i n c l u d e guard c o n t a i n s an ID f o r t h i s f i l e ”MATHSLIB ADDER H” , t h i s i s
// a r b i t r a r y but must be unique w i t h i n t h e p r o j e c t .

// begin = adder.h ============
#ifndef ADDER_H
#define ADDER_H

int add(int a, int b);

#endif // ADDER_H
// end = adder.h ============
