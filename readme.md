# Sharing equality of $\lambda$-graphs

A *reference implementation* of the algorithms presented in \[0\].

I documented my approach of building the shared $\lambda$-graph [in an
article](https://text.marvinborner.de/2023-05-30-16.html).

This project does not really have any purpose except for the
implementation itself. Its usage requires some adaption and
modification.

## Libraries

- [hashmap.c](https://github.com/tidwall/hashmap.c) \[MIT\]: Simple but
  efficient hashmap
- [xxHash](https://github.com/Cyan4973/xxHash/) \[BSD 2-Clause\]:
  Extremely fast hash algorithm

## References

\[0\]: Condoluci, Andrea, Beniamino Accattoli, and Claudio Sacerdoti
Coen. “Sharing equality is linear.” Proceedings of the 21st
International Symposium on Principles and Practice of Declarative
Programming. 2019.
