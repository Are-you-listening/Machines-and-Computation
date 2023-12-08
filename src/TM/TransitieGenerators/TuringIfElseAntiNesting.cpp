//
// Created by tibov on 07/12/23.
//

#include "TuringIfElseAntiNesting.h"

TuringIfElseAntiNesting::TuringIfElseAntiNesting(int split_nesting, int max_nesting): TuringGenerator(4), split_nesting{split_nesting}, max_nesting{max_nesting} {

}

IncompleteSet TuringIfElseAntiNesting::getTransitions() {
    return doAction();
}

IncompleteSet TuringIfElseAntiNesting::doAction() {
    IncompleteSet result{"IfElseAntiNesting", "IfElseAntiNesting"};

    //go to first potential place to do ifelse antinesting
    tools->go_to(result, {'O', 'U', '\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    string nothing_todo = tools->branch_on(result, {'\u0000'}, {get_tuple_index()[1]});

    //check if the function found starts with an if statement
    tools->go_to(result, {'{'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->move(result, get_tuple_index(), 1);
    tools->go_to(result, {'0','{'}, get_tuple_index()[1], 1, get_tuple_index());

    //makes loop if function not starting with 'if'
    tools->make_loop_on(result, '{', get_tuple_index()[1]);

    //in case starting with an if
    tools->link_put(result, {'S'}, {get_tuple_index()[0]});

    //mark first nesting as viable, if not, then remove marker
    tools->go_to(result, {'{'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(result, {'V'}, {get_tuple_index()[0]});
    tools->move(result, get_tuple_index(), 1);

    tools->go_to(result, {'{', '}'}, get_tuple_index()[1], 1, get_tuple_index());

    IncompleteSet notViableFirst{"notViableFirst", "notViableFirst"};
    tools->move(notViableFirst, get_tuple_index(), -1);
    tools->go_to(notViableFirst, {'{'}, get_tuple_index()[1], -1, get_tuple_index());
    tools->write_on(notViableFirst, {'V'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->skip_nesting(notViableFirst, tapes-1, 1, get_tuple_index()[1], 1, get_tuple_index());

    tools->link_on(result, notViableFirst, {'{'}, {get_tuple_index()[1]});

    tools->move(result, get_tuple_index(), 1);

    string on_else = tools->branch_on(result, {'1'}, {get_tuple_index()[1]});
    tools->make_loop(result);
    result.to_state = on_else;

    //checkviable 2
    tools->go_to(result, {'{'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(result, {'V'}, {get_tuple_index()[0]});
    tools->move(result, get_tuple_index(), 1);

    tools->go_to(result, {'{', '}'}, get_tuple_index()[1], 1, get_tuple_index());

    IncompleteSet notViableSecond{"notViableSecond", "notViableSecond"};
    tools->move(notViableSecond, get_tuple_index(), -1);
    tools->go_to(notViableSecond, {'{'}, get_tuple_index()[1], -1, get_tuple_index());
    tools->write_on(notViableSecond, {'V'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->skip_nesting(notViableSecond, tapes-1, 1, get_tuple_index()[1], 1, get_tuple_index());

    tools->link_on(result, notViableSecond, {'{'}, {get_tuple_index()[1]});

    //from here we will check at the only tokens after the if statements are spaces/returns/seperators
    tools->move(result, get_tuple_index(), 1);
    tools->go_to_not(result, {'R', 'E', 'S', 'P'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(result, {'E'}, {get_tuple_index()[0]});
    IncompleteSet clearViable{"clearViable", "clearViable"};
    tools->move(clearViable, get_tuple_index(), -1);
    tools->go_to_clear(clearViable, {'S'}, get_tuple_index()[0], -1, get_tuple_index(), {get_tuple_index()[0]});
    tools->go_to(clearViable, {'E'}, get_tuple_index()[0], 1, get_tuple_index());

    tools->link_on_not(result, clearViable, {'}'}, {get_tuple_index()[1]});

    tools->go_to(result, {'S', 'V'}, get_tuple_index()[0], -1, get_tuple_index());


    IncompleteSet onViable{"onViable", "onViable"};
    //clear all other viable
    tools->move(onViable, get_tuple_index(), -1);
    tools->go_to_clear(onViable, {'S'}, get_tuple_index()[0], -1, get_tuple_index(), {get_tuple_index()[0]});
    tools->go_to(onViable, {'V'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->makeAntiNesting(onViable, get_tuple_index());

    //on viable
    tools->link_on(result, onViable, {'V'}, {get_tuple_index()[0]});

    return result;
}