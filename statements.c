/*
 * Reads C source code from standard input and outputs the total count of:
 * (a) semi-colons ; 
 * (b) opening braces { or <%
 * that are not inside:
 * (1) C comments
 * (2) String or character literals
 * (3) Parentheses
 * Writes the total count to standard output as a decimal integer.
 * This is Jerry's change
 */


#include <stdio.h>

/*
 * Performs the same function as getchar() but automatically skips 
 * the line continuation character.
 * @return an int; the next character that is not a line continuation character
 */
int getcharskiplc();

/* Create a finite state machine with the following states:
 * (1) CODE = not in a C comment, character, or string literal
 * (2) COMMENT1 = in a C comment delimited by slash-asterisk
 * (3) COMMENT2 = in a C comment delimited by two slashes
 * (4) CHAR = in a character literal
 * (5) STRING = in a string literal
 */
enum state {CODE, COMMENT1, COMMENT2, CHAR, STRING};

int main(int arg, char *argc[])
{
    // current and previous states of the FSM, default state is CODE
    enum state curr_state = CODE;

    // total count of semi-colons and opening braces according to the assignment spec
    int count = 0;

    // keeps track of currently nested parentheses
    int nesting = 0;

    // currently read character
    int curr_char;

    // continue reading characters until end of input
    while(curr_char != EOF)
    {
        // read the next character and store in curr_char
        curr_char = getcharskiplc();
        switch(curr_state)
        {
            case CODE:
            if ((curr_char == ';' || curr_char == '{') && nesting == 0)
            {
                // not in comment, character or string literal, or parens (since nesting is 0)
                // increment count for semicolon and opening bracket
                count++;
            }
            else if (curr_char == '<')
            {
                curr_char = getcharskiplc();
                if (curr_char == '%' && nesting == 0)
                {
                    // increment for digraphs that are not in parens
                    count++;
                }
                else
                {
                    // not a digraph, so ungetc to avoid skipping on next read
                    ungetc(curr_char, stdin);
                }
            }
            else if (curr_char == '/')
            {
                curr_char = getcharskiplc();
                if (curr_char == '*')
                {
                    // COMMENT1 state triggered
                    curr_state = COMMENT1;
                }
                else if (curr_char == '/')
                {
                    // COMMENT2 state triggered
                    curr_state = COMMENT2;
                }
                else
                {
                    // not a relevant sequence, ungetc to avoid skipping on next getchar()
                    ungetc(curr_char, stdin);
                }
            }
            else if (curr_char == '\'')
            {
                // CHAR state triggered
                curr_state = CHAR;
            }
            else if (curr_char == '"')
            {
                // STRING state triggered
                curr_state = STRING;
            }
            else if (curr_char == '(')
            {
                // parens triggered, increment nesting
                nesting++;
            }
            else if (curr_char == ')')
            {
                // parens untriggered, decrement nesting
                nesting--; 
            }
            // all relevant triggers have been checked, read next character
            break;

            case COMMENT1:
            if (curr_char == '*')
            {
                curr_char = getcharskiplc();
                if (curr_char == '/')
                {
                    // terminate COMMENT1 state
                    curr_state = CODE;
                }
                else
                {
                    // not a relevant sequence, ungetc to avoid skipping on next read
                    ungetc(curr_char, stdin);
                }
            }
            // COMMENT1 state not terminated, continue reading
            break;

            case COMMENT2:
            if (curr_char == '\n')
            {
                // terminate COMMENT2 state once newline reached
                curr_state = CODE;
            }
            // COMMENT 2 state not terminated, continue reading
            break;

            case CHAR:
            if (curr_char == '\'')
            {
                 // terminate CHAR state 
                curr_state = CODE;
            }
            else if (curr_char == '\\')
            {
                // escape sequence, skip the next character and continue reading
                getcharskiplc();
            }
            // CHAR state not terminated, continue reading
            break;

            case STRING:
            if (curr_char == '"')
            {
                 // terminate STRING state
                curr_state = CODE;
            }
            else if (curr_char == '\\')
            {
                // escape sequence, skip the next character and continue reading
                getcharskiplc();
            }
            // STRING state not terminated, continue reading
            break;
        }
    }
    // print total semicolons and open brackets according to problem spec to standard output
    printf("%d\n", count);

    return 0;
}

int getcharskiplc()
{
    // reads and stores the current character
    int curr = getchar();

    // end of file must eventually be reached, so even if extremely long file of 
    // only line continuations, this while loop will eventually terminate
    while (curr == '\\')
    {
        // if current character is \, check if next character is a newline
        curr = getchar();
        if (curr == '\n')
        {
            // if a line continuation is found, read and store the next character and loop again
            curr = getchar();
        }
        else
        {
            // if a line continuation is not found, put the character back and return back-slash
            ungetc(curr, stdin);
            return '\\';
        }
    }
    // no \ present to trigger, simply return the read character
    return curr;
}
