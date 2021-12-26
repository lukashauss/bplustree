# bplustree

insert:
    - traverse to leaf node
    - insert value into leaf node
    - if overflow in leaf:
        - split leaf
        - copy keys and values
        - insert mid key into parent:
            
            -> insert key, add new child
            - loop:
                - if overflow:
                    - split node
                    - copy keys
                    - copy children
                    - insert mid into parent
