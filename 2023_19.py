import sys;
import logging;
import copy;

AOC_DAY=19
AOC_YEAR=2023



if('__main__' == __name__):
    import argparse;

    # deal with command line arguments
    parser = argparse.ArgumentParser(
            description = 'Advent of Code {} day {} solution'.format(
                AOC_YEAR, AOC_DAY));
    logger = logging.Logger('AOC{}d{}'.format(AOC_YEAR, AOC_DAY));
    logger.addHandler(logging.StreamHandler());

    parser.add_argument(
        '-T', '--test', action='store_true',
        help='test', default=False);
    parser.add_argument(
        '-2', '--part2', action='store_true',
        help='part 2?', default=False);
    parser.add_argument(
        '-v', '--verbose', action='store_true',
        help='be verbose', default=False);

    args = parser.parse_args();
    if(args.verbose):
        logger.setLevel(logging.DEBUG);
    else:
        logger.setLevel(logging.INFO);

    answer = 0;
    rules = {};
    filename = f"input/input_{AOC_YEAR}_{AOC_DAY}"
    if(args.test):
        filename = f"test/test_{AOC_YEAR}_{AOC_DAY}"
    with open(filename, 'r') as fh:
        lines = fh.readlines();
        lineidx = 0;

        # create a rule 
        #  each "rule" is a tuple, either length 4 or length 1. The
        #  last element in the tuple is always the destination.
        #  If there's a conditional in the rule, that'll be the first 3
        #  elements in the tuple:
        #    the name of the field to retrieve from the item
        #    '<' or '>'
        #    the integer value with which to compare

        while(lineidx < len(lines) and lines[lineidx].strip() != ""):
            # read rule lines
            line = lines[lineidx];
            lineidx += 1;
            print(f"Processing rule line {line}");

            rulename, rulestr = line.strip().strip('}').split('{');
            newrule = [];
            rules[rulename] = newrule;
            for rule in rulestr.split(','):
                # each of these might have:
                #  an expression like x>2662, followed by ':'
                #  a target: A, R, or <rulename>
                expr = rule.split(':');
                if(len(expr) > 1):  # found expression
                    if(expr[0][1] == '>'):
                        newrule.append((expr[0][0], '>', int(expr[0][2:]), expr[1]))
                    else:  # <
                        newrule.append((expr[0][0], '<', int(expr[0][2:]), expr[1]))

                else:  # no expression, just an action
                    newrule.append((expr[0],));

        # read items
        for line in lines[lineidx + 1:]:
            if(line.strip()) == '':
                break;
            print("")
            print(f"item line: {line}")
            item = {};
            for info in line.strip().strip('{}').split(','):
                item[info[0]] = int(info[2:])
            # run rules starting with 'in' until an action is reached
            rulename = 'in'
            while(rulename != 'A' and rulename != 'R'):
                print(f"  running rule {rulename} ({rules[rulename]}) on {item}");
                for rule in rules[rulename]:
                    if(len(rule) > 1):
                        print(f"     subrule {rule}");
                        # run conditional; if it matches, keep target
                        if(rule[1] == '>' \
                                and item.get(rule[0]) > rule[2]):
                            print("    found >");
                            rulename = rule[3];
                            break;
                        elif(rule[1] == '<' \
                                and item.get(rule[0]) < rule[2]):
                            print("    found <");
                            rulename = rule[3];
                            break;
                    else:
                        # no conditional, just keep target
                        rulename = rule[-1];
                        break;
            if(rulename == 'A'):
                print(f"keeping item {item}");
                answer += sum(item.values())


print(f"Final sum: {answer}");

