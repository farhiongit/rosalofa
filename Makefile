# See https://rosalind.info/problems/list-view/
SHELL=/bin/ksh
CFLAGS+=-fPIC

# --------- DEPENDENCIES ------------
DIRMAPS = ../minimaps# see https://github.com/farhiongit/minimaps
DIRAC75 = ../aho-corasick-1975# see https://github.com/farhiongit/aho-corasick-1975
CPPFLAGS += -I. -I$(DIRMAPS) -I$(DIRAC75)
LDFLAGS += -L. -L$(DIRMAPS) -L$(DIRAC75)
LDLIBS += -lac75 -lmap -lrosatools -lnewick -lm -lsuffixtree -ltrie

all: \
librosatools.so libnewick.so libsuffixtree.so libtrie.so \
dna rna revc fib gc hamm iprb prot subs cons fibd grph iev lcsm prob mrna perm prtm mprt splc sseq kmp revp orf lexf tree inod lgis pper pmch sign tran lexv \
rear sort long cat corr kmer sset lia lcsq mmch pdst rstr aspc edit edta eval motz nwck scsp seto spec trie conv ctbl dbru nkew itwv lrep suff mrep cstr indc

# --------- COMPILATION ------------
CFLAGS += -O3
#CFLAGS += -g

sort: rear
	rm -f "$@" && ln "$<" "$@"

edta: edit
	rm -f "$@" && ln "$<" "$@"

nkew: nwck
	rm -f "$@" && ln "$<" "$@"

mrep: mrepI mrepII

#cstr: CFLAGS+=-DTRIE

# Using uintbig_t:
fibd pmch cat motz mmch aspc: CFLAGS+=-Wno-format -Wno-format-security

# --------- TESTS ------------
test_%: %
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS):$(DIRAC75) ./$^ < "/home/laurent/Downloads/rosalind_$^.txt" > "/home/laurent/Downloads/rosalind_$^.out" 2>/dev/null

test_orf: orf
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS):$(DIRAC75) ./$^ < "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null | sort | uniq > "/home/laurent/Downloads/rosalind_$^.out"

test_corr: corr
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS):$(DIRAC75) ./$^ < "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null | sort | uniq > "/home/laurent/Downloads/rosalind_$^.out"

test_iprb: iprb
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS) ./$^ "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null

test_mprt: mprt
	while read -r uniprot ; do print ">$$uniprot" ; wget -q "https://rest.uniprot.org/uniprotkb/$${uniprot%%_*}.fasta" -O - ; done < "/home/laurent/Downloads/rosalind_$^.txt" | LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975 ./mprt 2>/dev/null

test_seto: seto
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS):$(DIRAC75) ./$^ < "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null | while IFS= read line ; do print -- "$${line%, \}} }" ; done > "/home/laurent/Downloads/rosalind_$^.out"

test_mrep: mrep
	LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975 ./mrepI 1 < "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null | LD_LIBRARY_PATH=.::../minimaps:../aho-corasick-1975 ./mrepII 20 > "/home/laurent/Downloads/rosalind_$^.out" 2>/dev/null

test_cstr: cstr
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}:$(DIRMAPS):$(DIRAC75) ./$^ < "/home/laurent/Downloads/rosalind_$^.txt" 2>/dev/null | sort > "/home/laurent/Downloads/rosalind_$^.out"

# --------- LIBRARIES ------------
lib%.so: LDFLAGS+=-shared
lib%.so: %.o
	$(CC) $(LDFLAGS) -o "$@" "$^"

.INTERMEDIATE: rosatools.o trie.o
rosatools.o: CFLAGS+=-DTU -Wno-format -Wno-format-security
rosatools.o: rosatools.h rosatools.c
trie.o: CFLAGS+=-DTU
trie.o: trie.c trie.h

