--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: taxa; Type: TABLE; Schema: public; Owner: huckfinn; Tablespace: 
--

CREATE TABLE taxa (
    tx_id serial primary key,
    tx_euring character varying(8) NOT NULL,
    tx_sea_flag boolean,
    tx_abv_de character varying(8) NOT NULL,
    tx_abv_dk character varying(8) NOT NULL,
    tx_name_lat character varying(64) NOT NULL,
    tx_name_de character varying(64) NOT NULL,
    tx_name_en character varying(64) NOT NULL,
    tx_name_dk character varying(64) NOT NULL
);


ALTER TABLE public.taxa OWNER TO huckfinn;

--
-- Name: TABLE taxa; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON TABLE taxa IS 'Artenliste ';


--
-- Name: COLUMN taxa.tx_id; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_id IS 'Taxa primary key';


--
-- Name: COLUMN taxa.tx_euring; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_euring IS 'EURING Kode ';


--
-- Name: COLUMN taxa.tx_sea_flag; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_sea_flag IS 'Fuer den Bildflug relevante Arten';


--
-- Name: COLUMN taxa.tx_abv_de; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_abv_de IS 'Kuerzel auf deutsch';


--
-- Name: COLUMN taxa.tx_abv_dk; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_abv_dk IS 'Kuerzel auf englisch';


--
-- Name: COLUMN taxa.tx_name_lat; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_name_lat IS 'Wissenschaftlicher Name';


--
-- Name: COLUMN taxa.tx_name_de; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_name_de IS 'Deutscher Name';


--
-- Name: COLUMN taxa.tx_name_en; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_name_en IS 'Englischer Name';


--
-- Name: COLUMN taxa.tx_name_dk; Type: COMMENT; Schema: public; Owner: huckfinn
--

COMMENT ON COLUMN taxa.tx_name_dk IS 'Daenischer Name';


--
-- Data for Name: taxa; Type: TABLE DATA; Schema: public; Owner: huckfinn
--

COPY taxa (tx_id, tx_euring, tx_sea_flag, tx_abv_de, tx_abv_dk, tx_name_lat, tx_name_de, tx_name_en, tx_name_dk) FROM stdin;
1	06549	f	av	alcsp	Alcidae indet.	Alkenvogel unbestimmt	Auklet sp.	uidenficeret alkefugl
2	05120	f	ap	caalp	Calidris alpina	Alpenstrandläufer	Dunlin	Almindelig Ryle
3	05121	f	apa	caala	Calidris alpina alpina	Alpenstrandläufer (Unterart alpina)	Dunlin (alpina)	Almindelig Ryle (alpina)
4	11870	f	a	tumer	Turdus merula	Amsel	Blackbird	Solsort
5	04500	f	af	haost	Haematopus ostralegus	Austernfischer	Oystercatcher	Strandskade
6	10200	f	ba	moalb	Motacilla alba	Bachstelze	White Wagtail	Hvid Vipstjert
7	13640	f	bam	pabia	Panurus biarmicus	Bartmeise	Bearded Tit	Skægmejse
8	00710	f	bt	subas	Sula bassana	Basstölpel	Northern Gannet	Sule
9	03100	f	bf	fasub	Falco subbuteo	Baumfalke	Hobby	Lærkefalk
10	10090	f	bp	antri	Anthus trivialis	Baumpieper	Tree Pipit	Skovpiber
11	05190	f	bek	gagal	Gallinago gallinago	Bekassine	Snipe	Dobbeltbekkasin
12	02040	f	bge	aymar	Aythya marila	Bergente	Greater Scaup	Bjergand
13	16380	f	ber	frmon	Fringilla montifringilla	Bergfink	Brambling	Kvækerfinke
14	16620	f	bhf	cafla	Carduelis flavirostris	Berghänfling	Twite	Bjergirisk
15	13070	f	bls	phbon	Phylloscopus bonelli	Berglaubsänger	Bonelli’s Warbler	Bjergløvsanger
16	14900	f	bem	repen	Remiz pendulinus	Beutelmeise	Penduline Tit	Pungmejse
17	08400	f	bie	meapi	Merops apiaster	Bienenfresser	Bee-eater	Biæder
18	16650	f	bks	loleu	Loxia leucoptera	Bindenkreuzschnabel	Two-barred Crossbill	Hvidvinget Korsnæb
19	16630	f	bir	carfl	Carduelis flammea	Birkenzeisig	Common Redpoll	Gråsisken
20	01590	f	bg	analb	Anser albifrons	Blässgans	Greater White-fronted Goose	Blisgås
21	04290	f	bh	fuatr	Fulica atra	Blässhuhn	Common Coot	Blishøne
22	11060	f	blk	lusve	Luscinia svecica	Blaukehlchen	Bluethroat	Blåhals
23	14620	f	bm	pacae	Parus caeruleus	Blaumeise	Blue Tit	Blåmejse
24	16600	f	hf	carca	Carduelis cannabina	Bluthänfling	Linnet	Tornirisk
25	05810	f	bom	laphi	Larus philadelphia	Bonapartemöwe	Bonaparte's Gull	Bonapartemåge
26	10050	f	br	ancam	Anthus campestris	Brachpieper	Tawny Pipit	Markpiber
27	01730	f	be	tatad	Tadorna tadorna	Brandgans	Shelduck	Gravand
28	06110	t	bs	stsan	Sterna sandvicensis	Brandseeschwalbe	Sandwich Tern	Splitterne
29	11370	f	bk	sarub	Saxicola rubetra	Braunkehlchen	Whinchat	Bynkefugl
30	05540	f	bru	trgla	Tringa glareola	Bruchwasserläufer	Wood Sandpiper	Tinksmed
31	16360	t	b	frcoe	Fringilla coelebs	Buchfink	Chaffinch	Bogfinke
32	08760	f	bsp	demaj	Picoides major	Buntspecht	Great Spotted Woodpecker	Stor Flagspætte
33	02919	f	bu	buspe	Buteo indet.	Bussard unbestimmt	Buzzard sp.	våge sp.
34	06350	f	dl	urlom	Uria lomvia	Dickschnabellumme	Brünnich’s Guillemot	Polarlomvie
35	15600	f	do	comon	Corvus monedula	Dohle	Eurasian Jackdaw	Allike
36	05200	f	ds	gamed	Gallinago media	Doppelschnepfe	Great Snipe	Tredækker
37	12750	f	dg	sycom	Sylvia communis	Dorngrasmücke	Whitethroat	Tornsanger
38	06020	f	dm	ritri	Rissa tridactyla	Dreizehenmöwe	Kittiwake	Ride
39	08980	f	dzs	detri	Picoides tridactylus	Dreizehenspecht	Three-toed Woodpecker	tretået Spætte
40	12069	f	drs	turdus	Turdidae indet.	Drossel unbestimmt	Thrush sp.	uidentificeret Drossel
41	12530	f	dr	acaru	Acrocephalus arundinaceus	Drosselrohrsänger	Great Reed Warbler	Drosselrørsanger
42	00430	f	dstu	pugri	Puffinus griseus	Dunkler Sturmtaucher	Sooty Shearwater	Sodfarvet Skråpe
43	05450	f	dw	trery	Tringa erythropus	Dunkler Wasserläufer	Spotted Redshank	Sortklire
44	15390	f	ei	gagla	Garrulus garrulus	Eichelhäher	Eurasien Jay	Skovskade
45	02060	t	ee	somol	Somateria mollissima	Eiderente	Common Eider	Ederfugl
46	02120	t	eie	clhye	Clangula hyemalis	Eisente	Long-tailed Duck	Havlit
47	05990	f	em	lahyp	Larus hyperboreus	Eismöwe	Glaucous Gull	Gråmåge
48	00220	t	es	fugla	Fulmarus glacialis	Eissturmvogel	Northern Fulmar	Mallemuk
49	00040	f	et	gaimm	Gavia immer	Eistaucher	Great Northern Diver	Islom
50	08310	f	ev	alatt	Alcedo atthis	Eisvogel	Kingfisher	Isfugl
51	15490	f	e	pipic	Pica pica	Elster	Black-billed Magpie	Husskade
52	02259	t	en	anasp	Anatinae indet.	Ente unbestimmt	Duck sp.	uidenficeret And
53	16540	f	ez	caspi	Carduelis spinus	Erlenzeisig	Siskin	Grønsisken
54	03219	f	fku	falsp	Falco indet.	Falke unbestimmt	Falcon sp.	falk sp
55	05680	f	frm	stlon	Stercorarius longicaudus	Falkenraubmöwe	Long-tailed Skua	Lille Kjove
56	03940	f	fa	phaco	Phasianus colchicus	Fasan	Pheasant	Fasan
57	09760	f	fl	alarv	Alauda arvensis	Feldlerche	Skylark	Sanglærke
58	12360	f	fsl	lonae	Locustella naevia	Feldschwirl	Grasshopper Warbler	Græshoppesanger
59	15980	f	fe	pamon	Passer montanus	Feldsperling	Tree Sparrow	Skovspurv
60	16660	f	fk	locur	Loxia curvirostra	Fichtenkreuzschnabel	Common Crossbill	Lille Korsnæb
61	16389	f	fi	unfin	Fringilla indet.	Fink unbestimmt	Chaffinch sp.	finke sp
62	03010	f	fd	pahal	Pandion haliaetus	Fischadler	Osprey	Fiskeørn
63	13120	f	f	phtro	Phylloscopus trochilus	Fitis	Willow Warbler	Løvsanger
64	06169	t	fks	sthip	Sterna hirundo / paradisaea	Fluss-/Küstenseeschwalbe	Common/Arctic Tern	Fjordterne / Havterne
65	04690	f	frp	chdub	Charadrius dubius	Flussregenpfeifer	Little Ringed Plover	Lille Præstekrave
66	06150	f	fs	sthir	Sterna hirundo	Flussseeschwalbe	Common Tern	Fjordterne
67	05560	f	fu	achyp	Actitis hypoleucos	Flussuferläufer	Common Sandpiper	Mudderklire
68	01699	t	gä	anspe	Anserini indet.	Gans unbestimmt	Goose sp.	uidenficeret Gås
69	02230	t	gän	memer	Mergus merganser	Gänsesäger	Goosander	Stor Skallesluger
70	14870	f	gb	cebra	Certhia brachydactyla	Gartenbaumläufer	Short-toed Treecreeper	Korttået træløber
71	12760	f	ggm	sybor	Sylvia borin	Gartengrasmücke	Garden Warbler	Havesanger
72	11220	f	gr	phoph	Phoenicurus phoenicurus	Gartenrotschwanz	Redstart	Rødstjert
73	10190	f	ge	mocin	Motacilla cinerea	Gebirgsstelze	Grey Wagtail	Bjergvipstjert
74	13000	f	gbl	phino	Phylloscopus inornatus	Gelbbrauen-Laubsänger	Yellow-browed Warbler	Hvidbrynet Løvsanger
75	00050	f	gse	gaadm	Gavia adamsii	Gelbschnabel-Eistaucher	White-billed Diver	Hvidnæbbet lom
76	00360	f	ges	cadio	Calonectris diomedea	Gelbschnabel-Sturmtaucher	Cory’s Shearwater	Kuhls Skråpe
77	12590	f	gp	hiict	Hippolais icterina	Gelbspötter	Icterine Warbler	Gulbug
78	03180	f	ger	farus	Falco rusticolus	Gerfalke	Gyrfalcon	Jagtfalk
79	17100	f	d	pypyr	Pyrrhula pyrrhula	Gimpel	Bullfinch	Dompap
80	16400	f	gi	seser	Serinus serinus	Girlitz	European Serin	Gulirisk
81	04869	f	gokir	plaps	Pluvialis apricaria / squatarola	Gold-/Kiebitzregenpfeifer	Golden/Grey Plover	Hjejle / Strandhjejle
82	18570	f	g	emcit	Emberiza citrinella	Goldammer	Yellowhammer	Gulspurv
83	13169	f	gos	respe	Regulus indet.	Goldhähnchen unbestimmt	Kinglet sp.	Fuglekonge sp.
84	12980	f	ghl	phpro	Phylloscopus proregulus	Goldhähnchen-Laubsänger	Pallas‘s Warbler	Fuglekongesanger
85	04850	f	go	plapr	Pluvialis apricaria	Goldregenpfeifer	Golden Plover	Hjejle
86	18820	f	ga	emcal	Miliaria calandra	Grauammer	Corn Bunting	Bomlærke
87	05070	f	gbs	camel	Calidris melanotos	Graubrust-Strandläufer	Pectoral Sandpiper	Stribet ryle
88	01610	t	gg	anans	Anser anser	Graugans	Greylag Goose	Grågås
89	01220	f	fr	arcin	Ardea cinerea	Graureiher	Common Heron	Fiskehejre
90	13350	f	grs	mustr	Muscicapa striata	Grauschnäpper	Spotted Flycatcher	Grå Fluesnapper
91	08550	f	gra	pican	Picus canus	Grauspecht	Grey-headed Woodpecker	Gråspætte
92	02748	t	gv	rovsp	Accipitridae indet.	Greifvogel unbestimmt	Bird of prey sp.	uidenficeret rovfugl
93	00950	f	rod	boste	Botaurus stellaris	Große Rohrdommel	Eurasian Bittern	Rørdrum
94	05410	f	gbr	nuarq	Numenius arquata	Großer Brachvogel	Curlew	Stor Regnspove
95	06005	t	gm	larus	Larus spec.	Großmöwe unbestimmt	Large gull sp.	uidenficeret stormåge
96	16490	f	gf	cachl	Carduelis chloris	Grünfink	Greenfinch	Grønirisk
97	12930	f	grl	phytr	Phylloscopus trochiloides	Grünlaubsänger	Greenish Warbler	Lundsanger
98	05480	f	gs	trneb	Tringa nebularia	Grünschenkel	Greenshank	Hvidklire
99	08560	f	gsp	pivir	Picus viridis	Grünspecht	Green Woodpecker	Grønspætte
100	06380	f	gry	cegry	Cepphus grylle	Gryllteiste	Black Guillemot	Tejst
101	02670	f	ha	acgen	Accipiter gentilis	Habicht	Northern Goshawk	Duehøg
102	13480	f	hb	fialb	Ficedula albicollis	Halsbandschnäpper	Collared Flycatcher	Hvidhalset Fluesnapper
103	09720	f	hl	gacri	Galerida cristata	Haubenlerche	Crested Lark	Toplærke
104	14540	f	ham	pacri	Parus cristatus	Haubenmeise	Crested Tit	Topmejse
105	00090	t	hat	pocri	Podiceps cristatus	Haubentaucher	Great Crested Grebe	Toppet Lappedykker
106	11210	f	hr	phoch	Phoenicurus ochruros	Hausrotschwanz	Black Redstart	Husrødstjert
107	15910	f	h	padom	Passer domesticus	Haussperling	House Sparrow	Gråspurv
108	06655	f	hx	co'do	Columba livia domestica	Haustaube	Feral Pigeon	Tamdue
109	10840	f	he	prmod	Prunella modularis	Heckenbraunelle	Dunnock	Jernspurv
110	09740	f	hei	luarb	Lullula arborea	Heidelerche	Woodlark	Hedelærke
111	06008	t	hmm	lafum	Larus fuscus / marinus	Herings-/Mantelmöwe	Great/Lesser Black-backed Gull	Sildemåge / Svartbag 
112	05919	f	hsm	lafug	Larus argentatus / fuscus	Herings-/Silbermöwe	Herring/Lesser Black-backed Gull	Silde-/Sølvmåge
113	05910	t	hm	lafus	Larus fuscus	Heringsmöwe	Lesser Black-backed Gull	Sildemåge
114	01520	f	hö	cyolo	Cygnus olor	Höckerschwan	Mute Swan	Knopsvane
115	06680	f	hot	cooen	Columba oenas	Hohltaube	Stock Dove	Huldue
116	05170	f	ka	phpug	Philomachus pugnax	Kampfläufer	Ruff	Brushane
117	01660	f	kag	brcan	Branta canadensis	Kanadagans	Canada Goose	Kanadagås
118	16790	f	kar	caery	Carpodacus erythrinus	Karmingimpel	Common Rosefinch	Karmindompap
119	17170	f	kb	cococ	Coccothraustes coccothraustes	Kernbeißer	Hawfinch	Kærnebider
120	04930	f	ki	vavan	Vanellus vanellus	Kiebitz	Lapwing	Vibe
121	04860	f	kir	plsqu	Pluvialis squatarola	Kiebitzregenpfeifer	Grey Plover	Strandhjejle
122	16680	f	kks	lopyt	Loxia pytyopsittacus	Kiefernkreuzschnabel	Parrot Crossbill	Stor Korsnæb
123	12740	f	kgm	sycur	Sylvia curruca	Klappergrasmücke	Lesser Whitethroat	Gærdesanger
124	14790	f	kl	sieur	Sitta europaea	Kleiber	Nuthatch	Spætmejse
125	04100	f	ksh	popar	Porzana parva	Kleines Sumpfhuhn	Little Crake	Lille Rørvagtel
126	05909	t	km	laris	Larus / Rissa indet.	Kleinmöwe unbestimmt	Lesser gull sp.	uidenficeret småmåge
127	08870	f	ksp	demin	Picoides minor	Kleinspecht	Lesser Spotted Woodpecker	Lille Flagspætte
128	01910	f	kne	anque	Anas querquedula	Knäkente	Garganey	Atlingand
129	04960	f	knt	cacan	Calidris canutus	Knutt	Knot	Islandsk Ryle
130	14640	f	ko	pamaj	Parus major	Kohlmeise	Great Tit	Musvit
131	01960	f	koe	neruf	Netta rufina	Kolbenente	Red-crested Pochard	Rødhovedet and
132	15720	f	kra	corco	Corvus corax	Kolkrabe	Common Raven	Ravn
133	00720	t	k	phcar	Phalacrocorax carbo	Kormoran	Great Cormorant	Skarv
134	02610	t	kw	cicya	Circus cyaneus	Kornweihe	Hen Harrier	Blå Kærhøg
135	06470	f	krt	alall	Alle alle	Krabbentaucher	Little Auk	Søkonge
136	15749	t	krä	cospe	Corvus indet.	Krähe unbestimmt	Crow sp.	uidenficeret Kragefugl
137	00800	f	krs	phari	Phalacrocorax aristotelis	Krähenscharbe	Shag Cormorant	Topskarv
138	04330	f	kch	grgru	Grus grus	Kranich	Common Crane	Trane
139	01840	f	kre	ancre	Anas crecca	Krickente	Green-winged Teal	Krikand
140	07240	f	ku	cucan	Cuculus canorus	Kuckuck	Cuckoo	Gøg
141	01110	f	kur	buibi	Bubulcus ibis	Kuhreiher	Cattle Egret	Kohejre
142	01580	f	kg	anbra	Anser brachyrhynchus	Kurzschnabelgans	Pink-footed Goose	Kortnæbbet Gås
143	06160	t	ks	stepa	Sterna paradisaea	Küstenseeschwalbe	Arctic Tern	Havterne
144	05820	t	lm	larid	Larus ridibundus	Lachmöwe	Black-headed Gull	Hættemåge
145	06050	f	las	genil	Gelochelidon nilotica	Lachseeschwalbe	Gull-billed Tern	Sandterne
146	00129	t	lt	pospe	Podicipedidae indet.	Lappentaucher unbestimmt	Grebe sp.	uidenficeret Lappedykker
147	13129	f	ls	physp	Phylloscopus indet.	Laubsänger unbestimmt	Leef Warbler sp.	uidenficeret Sanger
148	01940	f	le	ancly	Anas clypeata	Löffelente	Northern Shoveler	Skeand
149	01440	f	lf	plleu	Platalea leucorodia	Löffler	Common Spoonbill	Skestork
150	06000	t	mm	lamar	Larus marinus	Mantelmöwe	Great Black-backed Gull	Svartbag
151	07950	f	ms	apapu	Apus apus	Mauersegler	Swift	Mursejler
152	02870	f	mb	bubut	Buteo buteo	Mäusebussard	Eurasian Buzzard	Musvåge
153	05100	f	mst	camar	Calidris maritima	Meerstrandläufer	Purple Sandpiper	Sortgrå Ryle
154	10010	f	m	deurb	Delichon urbica	Mehlschwalbe	House Martin	Bysvale
155	03090	t	mer	facol	Falco columbarius	Merlin	Merlin	Dværgfalk
156	02399	f	mu	mispe	Milvus indet.	Milan unbestimmt	Kite sp.	Glente sp.
157	12020	f	mi	tuvis	Turdus viscivorus	Misteldrossel	Mistle Thrush	Misteldrossel
158	05926	t	wmm	lacam	Larus cachinnans michahellis	Mittelmeermöwe	Yellow-legged Gull (michahelis)	Middelhavssølvmåge
159	00463	f	mms	pupum	Puffinus puffinus mauretanicus	Mittelmeer-Sturmtaucher	Balearic Shearwater	Balearskråpe
160	02210	f	mis	meser	Mergus serrator	Mittelsäger	Red-breasted Merganser	Toppet Skallesluger
161	08830	f	msp	demed	Picoides medius	Mittelspecht	Middle Spotted Woodpecker	Mellemflagspætte
162	12770	f	mg	syatr	Sylvia atricapilla	Mönchsgrasmücke	Blackcap	Munk
163	02020	f	moe	aynyr	Aythya nyroca	Moorente	Ferrugineous Duck	Hvidøjet And
164	04820	f	mrp	chmor	Charadrius morinellus	Mornellregenpfeifer	Dotterel	Pomeransfugl
165	06049	t	mö	larsp	Laridae indet.	Möwe unbestimmt	Gull sp.	uidenficeret Måge
166	11040	f	n	lumeg	Luscinia megarhynchos	Nachtigall	Nightingale	Sydlig nattergal
167	01040	f	nr	nynyc	Nycticorax nycticorax	Nachtreiher	Black-crowned Night Heron	Nathejre
168	15673	f	ne	cocox	Corvus corone cornix	Nebelkrähe	Hooded Crow	Gråkrage
169	15150	f	nt	lacol	Lanius collurio	Neuntöter	Red-backed Shrike	Rødrygget Tornskade
170	01700	f	nig	alaeg	Alopochen aegyptiacus	Nilgans	Egyptian Goose	Nilgås
171	01670	t	ng	brleu	Branta leucopsis	Nonnengans	Barnacle Goose	Bramgås
172	05640	f	od	phlob	Phalaropus lobatus	Odinshühnchen	Red-necked Phalarope	Odinshane
173	09780	f	ol	eralp	Eremophila alpestris	Ohrenlerche	Shore Lark	Bjerglærke
174	00110	f	ot	poaur	Podiceps auritus	Ohrentaucher	Slavonian Grebe	Nordisk Lappedykker
175	18660	f	o	emhor	Emberiza hortulana	Ortolan	Ortolan Bunting	Hortulan
176	06540	f	pt	frarc	Fratercula arctica	Papageitaucher	Puffin	Lunde
177	01790	f	pe	anpen	Anas penelope	Pfeifente	Eurasian Wigeon	Pibeand
178	05340	f	ps	lilap	Limosa lapponica	Pfuhlschnepfe	Bar-tailed Godwit	Lille Kobbersneppe
179	10159	f	psp	antsp	Motacillidae indet.	Pieper unbestimmt	Pipit sp.	uidenficeret Piber
180	15080	f	p	orori	Oriolus oriolus	Pirol	Golden Oriole	Pirol
181	16640	f	pbz	cahor	Carduelis hornemanni	Polarbirkenzeisig	Arctic Redpoll	Hvidsisken
182	05980	f	pm	lagla	Larus glaucoides	Polarmöwe	Iceland Gull	Hvidvinget Måge
183	02070	f	pre	sospe	Somateria spectabilis	Prachteiderente	King Eider	Kongeederfugl
184	00030	f	ptt	gaarc	Gavia arctica	Prachttaucher	Black-throated Diver	Sortstrubet Lom
185	01240	f	pur	armel	Ardea purpurea	Purpurreiher	Purple Heron	Purpurhejre
186	15671	f	ra	cocoe	Corvus corone corone	Rabenkrähe	Carrion Crow	Sortkrage
187	05709	f	rm	skua	Stercorariidae indet.	Raubmöwe unbestimmt	Skua sp.	uidenficeret Kjove
188	06060	t	rbs	stcas	Sterna caspia	Raubseeschwalbe	Caspian Tern	Rovterne
189	15200	f	rw	laexc	Lanius excubitor	Raubwürger	Great Grey Shrike	Stor Tornskade
190	09920	f	r	hirus	Hirundo rustica	Rauchschwalbe	Swallow	Landsvale
191	02900	f	rb	bulag	Buteo lagopus	Rauhfußbussard	Rough-legged Buzzard	Fjeldvåge
192	03670	f	reb	peper	Perdix perdix	Rebhuhn	Grey Partridge	Agerhøne
193	05380	f	rbr	nupha	Numenius phaeopus	Regenbrachvogel	Whimbrel	Lille Regnspove
194	04839	f	ru	chspe	Charadrius indet.	Regenpfeifer unbestimmt	Plover sp.	Præstekrave sp
195	02030	t	re	ayful	Aythya fuligula	Reiherente	Tufted Duck	Troldand
196	11860	f	rdr	tutor	Turdus torquatus	Ringdrossel	Ring Ouzel	Ringdrossel
197	01680	f	rg	brber	Branta bernicla	Ringelgans	Brent Goose	Knortegås
198	1683	f	rgn	brben	Branta bernicla nigricans	Ringelgans (Unterart nigricans)	Black-bellied Brent Goose	Sortbuget Knortegås
199	06700	f	rt	copal	Columba palumbus	Ringeltaube	Woodpigeon	Ringdue
200	18770	f	ro	emsch	Emberiza schoeniclus	Rohrammer	Reed Bunting	Rørspurv
201	12549	f	rsp	acspe	Acrocephalus indet.	Rohrsänger unbestimmt	Reed-warblers	Acrosephalus sp
202	12380	f	ros	lolus	Locustella luscinioides	Rohrschwirl	Savi’s Warbler	Savisanger
203	02600	f	row	ciaer	Circus aeruginosus	Rohrweihe	Marsh Harrier	Rørhøg
204	01710	f	rog	tafer	Tadorna ferruginea	Rostgans	Ruddy Shelduck	Rustand
205	12010	t	rd	tuili	Turdus iliacus	Rotdrossel	Redwing	Vindrossel
206	03070	f	rff	faves	Falco vespertinus	Rotfußfalke	Red-footed Falcon	Aftenfalk
207	01690	f	rhg	brruf	Branta ruficollis	Rothalsgans	Red-breasted Goose	Rødhalset gås
208	00100	f	rht	pogri	Podiceps grisegena	Rothalstaucher	Red-necked Grebe	Gråstrubet Lappedykker
209	10990	f	rk	errub	Erithacus rubecula	Rotkehlchen	Robin	Rødhals
210	10120	f	rkp	ancer	Anthus cervinus	Rotkehlpieper	Red-throated Pipit	Rødstrubet Piber
211	15230	f	rkw	lasen	Lanius senator	Rotkopfwürger	Woodchat Shrike	Rødhovedet Tornskade
212	02390	f	rom	mimil	Milvus milvus	Rotmilan	Red Kite	Rød Glente
213	05460	f	rs	trtot	Tringa totanus	Rotschenkel	Redshank	Rødben
214	11061	f	blkr	lussv	Luscinia svecica svecica	Rotsterniges Blaukehlchen	Red-spotted Bluethroat	Nordlig Blåhals
215	11050	f	rbk	lucal	Luscinia calliope	Rubinkehlchen	Siberian Rubythroat	Rubinnattergal
216	01570	f	sg	anfab	Anser fabalis	Saatgans	Bean Goose	Sædgås
217	15630	f	sa	cofru	Corvus frugilegus	Saatkrähe	Rook	Råge
218	04560	f	sb	reavo	Recurvirostra avosetta	Säbelschnäbler	Avocet	Klyde
219	02239	f	sä	mersp	Mergus indet.	Säger unbestimmt	Merganser sp.	uidenficeret Skallesluger
220	02150	t	se	mefus	Melanitta fusca	Samtente	Velvet Scoter	Fløjlsand
221	04970	f	san	caalb	Calidris alba	Sanderling	Sanderling	Sandløber
222	04700	f	srp	chhia	Charadrius hiaticula	Sandregenpfeifer	Ringed Plover	Stor Præstekrave
223	10170	f	sz	mofla	Motacilla flava	Schafstelze	Yellow Wagtail	Gul Vipstjert
224	02090	f	sce	poste	Polysticta stelleri	Scheckente	Steller's Eider	Stellersand
225	02930	f	sca	aqcla	Aquila clanga	Schelladler	Spotted Eagle	Stor Skrigeørn
226	02180	f	sle	bucla	Bucephala clangula	Schellente	Common Goldeneye	Hvinand
227	12430	f	sr	acsch	Acrocephalus schoenobaenus	Schilfrohrsänger	Sedge Warbler	Sivsanger
228	12370	f	ssc	loflu	Locustella fluviatilis	Schlagschwirl	River Warbler	Flodsanger
229	02560	f	sla	cigal	Circaetus gallicus	Schlangenadler	Short-toed Eagle	Slangeørn
230	07350	f	sel	tyalb	Tyto alba	Schleiereule	Barn Owl	Slørugle
231	05699	f	ssf	stlpm	Stercorarius longicaudus/parasiticus/pomarinus	Schmarotzer-/Spatel-/Falkenraubmöwe	Long-tailed/Arctic/Pomarine Skua	Lille Kjove/Almindelig Kjove/ Mellemkjove
232	05698	f	ssrm	stpap	Stercorarius parasiticus / pomarinus	Schmarotzer-/Spatelraubmöwe	Arctic/Pomarine Skua	Almindelig / Mellemkjove
233	05670	f	srm	stpar	Stercorarius parasiticus	Schmarotzerraubmöwe	Arctic Skua	Almindelig Kjove
234	01820	f	sne	anstr	Anas strepera	Schnatterente	Gadwall	Knarand
235	18500	f	sna	plniv	Plectrophenax nivalis	Schneeammer	Snow Bunting	Snespurv
236	07490	f	seu	nysca	Nyctea scandiaca	Schnee-Eule	Snowy Owl	Sneugle
237	01630	f	sng	ancae	Anser caerulescens	Schneegans	Snow Goose	Snegås
238	02920	f	sra	aqpom	Aquila pomarina	Schreiadler	Lesser Spotted Eagle	Lille skrigeørn
239	10019	f	scu	hirsp	Hirundinidae indet.	Schwalbe unbestimmt	Swallow sp.	Ubestemt svale
240	05790	f	scm	lasab	Larus sabini	Schwalbenmöwe	Sabine’s Gull	Sabinemåge
241	01559	t	sch	cyspe	Cygnidae indet.	Schwan unbestimmt	Swan sp.	uidenficeret Svane
242	14370	f	sme	aecau	Aegithalos caudatus	Schwanzmeise	Long-tailed Tit	Halemejse
243	00140	f	sba	dimel	Diomedea melanophris	Schwarzbrauenalbatros	Black-browed Mollymawk	Sortbrynet Albatros
244	00120	f	sht	ponig	Podiceps nigricollis	Schwarzhalstaucher	Black-necked Grebe	Sorthalset Lappedykker
245	11390	f	sk	sator	Saxicola torquata	Schwarzkehlchen	Stonechat	Sortstrubet Bynkefugl
246	05750	f	skm	lamel	Larus melanocephalus	Schwarzkopfmöwe	Mediterranean Gull	Sorthovedet Måge
247	02250	f	ske	oxjam	Oxyura jamaicensis	Schwarzkopf-Ruderente	Ruddy Duck	Amerikansk Skarveand
248	02380	f	swm	mimig	Milvus migrans	Schwarzmilan	Black Kite	Sort Glente
249	00460	f	sst	pupuf	Puffinus puffinus	Schwarzschnabel-Sturmtaucher	Manx Shearwater	Almindelig Skråpe
250	08630	f	ssp	drmar	Dryocopus martius	Schwarzspecht	Black Woodpecker	Sortspætte
251	01310	f	sso	cinig	Ciconia nigra	Schwarzstorch	Black Stork	Sort stork
252	02430	f	sea	haalb	Haliaeetus albicilla	Seeadler	White-tailed Eagle	Havørn
253	04770	f	ser	chale	Charadrius alexandrinus	Seeregenpfeifer	Kentish Plover	Hvidbrystet Præstekrave
254	06299	t	ss	stnsp	Sterninae indet.	Seeschwalbe unbestimmt	Tern sp.	uidenficeret Terne
255	00059	t	st	gavia	Gaviidae indet.	Seetaucher unbestimmt	Diver sp.	uidenficeret Lom
256	12420	f	seg	acrpa	Acrocephalus paludicola	Seggenrohrsänger	Aquatic Warbler	Vandsanger
257	01190	f	sed	eggar	Egretta garzetta	Seidenreiher	Little Egret	Silkehejre
258	10480	f	sei	bogar	Bombycilla garrulus	Seidenschwanz	Waxwing	Silkehale
259	05090	f	sis	cafer	Calidris ferruginea	Sichelstrandläufer	Curlew Sandpiper	Krumnæbbet Ryle
260	05929	f	ssm	lacaa	Larus argentatus / canus	Silber-/Sturmmöwe	Herring/Common Gull	Sølvmåge / Stormmåge
261	05920	t	sm	laarg	Larus argentatus	Silbermöwe	Herring Gull	Sølvmåge
262	01210	t	sir	egalb	Egretta alba	Silberreiher	Great White Egret	Sølvhejre
263	12000	f	si	tuphi	Turdus philomelos	Singdrossel	Song Thrush	Sangdrossel
264	01540	f	siw	cycyg	Cygnus cygnus	Singschwan	Whooper Swan	Sangsvane
265	19999	f	sv	unpas	Passeriformes indet.	Singvogel unbestimmt	Passerine sp.	uidenficeret spurvefugl
266	05690	f	sku	stsku	Stercorarius skua	Skua	Skua	Storkjove
267	13150	f	sgh	reign	Regulus ignicapillus	Sommergoldhähnchen	Firecrest	Rødtoppet Fuglekonge
268	02170	f	spt	buisl	Bucephala islandica	Spatelente	Barrow’s Goldeneye	Islandsk Hvinand
269	05660	f	spm	stpom	Stercorarius pomarinus	Spatelraubmöwe	Pomarine Skua	Mellemkjove
270	02690	f	sp	acnis	Accipiter nisus	Sperber	European Sparrow Hawk	Spurvehøg
271	12730	f	sgm	synis	Sylvia nisoria	Sperbergrasmücke	Barred Warbler	Høgesanger
272	07510	f	spk	glpas	Glaucidium passerinum	Sperlingskauz	Pygmy Owl	Spurveugle
273	01890	f	spe	anacu	Anas acuta	Spießente	Northern Pintail	Spidsand
274	18470	f	spa	calap	Calcarius lapponicus	Spornammer	Lapland Longspur	Laplandsværling
275	10020	f	spp	anric	Anthus richardi	Spornpieper	Richard’s Pipit	Storpiber
276	11030	f	spr	lulus	Luscinia luscinia	Sprosser	Thrush Nightingale	Noordse Nachtegaal
277	15820	f	s	stvul	Sturnus vulgaris	Star	Common Starling	Stær
278	02960	f	sta	aqchr	Aquila chrysaetos	Steinadler	Golden Eagle	Kongeørn
279	07570	f	stk	atnoc	Athene noctua	Steinkauz	Little Owl	Kirkeugle
280	11460	f	sts	oeoen	Oenanthe oenanthe	Steinschmätzer	Wheatear	Stenpikker
281	05610	f	sw	arint	Arenaria interpres	Steinwälzer	Turnstone	Stenvender
282	04550	f	stf	hihim	Himantopus himantopus	Stelzenläufer	Black-winged Stilt	Stylteløber
283	05939	t	wms	lacac	Larus cachinnans cachinnans	Steppenmöwe	Caspian Gull	Kaspisk Måge
284	02620	f	spw	cimac	Circus macrourus	Steppenweihe	Pallid Harrier	Steppehøg
285	00020	t	stt	gaste	Gavia stellata	Sterntaucher	Red-throated Diver	Rødstrubet Lom
286	16530	f	sti	cacar	Carduelis carduelis	Stieglitz	Goldfinch	Stillits
287	01860	f	ste	anpla	Anas platyrhynchos	Stockente	Mallard	Gråand
288	05129	f	stl	caspe	Calidris indet.	Strandläufer unbestimmt	Stint sp.	uidenficeret Ryle
289	10143	f	stp	anlit	Anthus spinoletta littoralis	Strandpieper (Unterart littoralis)	Scandinavian Rock Pipit	Skærpiber
290	01620	f	srg	anind	Anser indicus	Streifengans	Bar-headed Goose	Indisk gås
291	05900	t	stm	lacan	Larus canus	Sturmmöwe	Common Gull	Stormmåge
292	00520	f	stw	hypel	Hydrobates pelagicus	Sturmschwalbe	European Storm Petrel	Lille Stormsvale
293	00499	f	stu	puspe	Puffinus indet.	Sturmtaucher unbestimmt	Shearwater sp.	uidenficeret Skråpe
294	05140	f	sul	lifal	Limicola falcinellus	Sumpfläufer	Broad-billed Sandpiper	Kærløber
295	14400	f	sum	papal	Parus palustris	Sumpfmeise	Marsh Tit	Sumpmejse
296	07680	f	soe	asfla	Asio flammeus	Sumpfohreule	Short-eared Owl	Mosehornugle
297	12500	f	su	acpal	Acrocephalus palustris	Sumpfrohrsänger	Marsh Warbler	Kærsanger
298	01980	f	tfe	ayfer	Aythya ferina	Tafelente	Common Pochard	Taffeland
299	15570	f	tah	nucar	Nucifraga caryocatactes	Tannenhäher	Spotted Nutcracker	Nøddekrige
300	14610	f	tm	paate	Parus ater	Tannenmeise	Coal Tit	Sortmejse
301	06999	f	tb	colsp	Columbidae indet.	Taube unbestimmt	Dove sp.	uidenficeret Due
302	04240	f	th	gachl	Gallinula chloropus	Teichhuhn	Moorhen	Grønbenet Rørhøne
303	12510	f	t	acsci	Acrocephalus scirpaceus	Teichrohrsänger	Reed Warbler	Rørsanger
304	05470	f	twl	trsta	Tringa stagnatilis	Teichwasserläufer	Marsh Sandpiper	Damklire
305	05020	f	tem	catem	Calidris temminckii	Temminckstrandläufer	Temminck’s Stint	Temmincksryle
306	05550	f	tkl	xecin	Xenus cinereus	Terekwasserläufer	Terek Sandpiper	Terekklire
307	5970	f	tym	latha	Larus thayeri	Thayer-Möwe	Thayer´s Gull	Inuitmåge
308	06360	t	ta	altor	Alca torda	Tordalk	Razorbill	Alk
309	02139	f	tse	mespe	Melanitta indet.	Trauer-/Samtente	Black/Velvet Scoter	Sortand / Fløjlsand
310	02130	t	te	menig	Melanitta nigra	Trauerente	Common Scoter	Sortand
311	13490	f	trs	fihyp	Ficedula hypoleuca	Trauerschnäpper	Pied Flycatcher	Broget Fluesnapper
312	06270	f	ts	chnig	Chlidonias niger	Trauerseeschwalbe	Black Tern	Sortterne 
313	04590	f	tri	buoed	Burhinus oedicnemus	Triel	Stone Curlew	Triel
314	06340	t	tl	uraal	Uria aalge	Trottellumme	Guillemot	Lomvie
315	06345	t	tt	alkur	Alca torda / Uria aalge	Trottellumme/Tordalk	Common Guillemot/Razorbill	Lomvie / Alk
316	04080	f	tsu	popor	Porzana porzana	Tüpfelsumpfhuhn	Spotted Crake	Plettet Rørvagtel
317	06840	f	tüt	stdec	Streptopelia decaocto	Türkentaube	Collared Dove	Tyrkerdue
318	03040	t	tf	fatin	Falco tinnunculus	Turmfalke	Eurasian Kestrel	Tårnfalk
319	06870	f	tut	sttur	Streptopelia turtur	Turteltaube	Turtle Dove	Turteldue
320	05320	f	us	lilim	Limosa limosa	Uferschnepfe	Black-tailed Godwit	Stor Kobbersneppe
321	09810	f	uf	ririp	Riparia riparia	Uferschwalbe	Sand Martin	Digesvale
322	15430	f	ugh	peinf	Perisoreus infaustus	Unglückshäher	Siberian Jay	Lavskrige
323	11980	f	wd	tupil	Turdus pilaris	Wacholderdrossel	Fieldfare	Sjagger
324	03700	f	wa	cocot	Coturnix coturnix	Wachtel	Quail	Vagtel
325	04210	f	wk	crcre	Crex crex	Wachtelkönig	Corncrake	Engsnarrer
326	07689	f	wsoe	asofl	Asio otus / flammeus	Wald-/Sumpfohreule	Long-eared/Short eared Owl	Skov- / Mosehornugle
327	14860	f	wb	cefam	Certhia familiaris	Waldbaumläufer	Treecreeper	Træløber
328	07610	f	wz	stalu	Strix aluco	Waldkauz	Tawny Owl	Natugle
329	13080	f	wls	phsib	Phylloscopus sibilatrix	Waldlaubsänger	Wood Warbler	Skovsanger
330	07670	f	woe	asotu	Asio otus	Waldohreule	Long-eared Owl	Skovhornugle
331	10080	f	wap	anhod	Anthus hodgsoni	Waldpieper	Olive-backed Pipit	Taigapiber
332	05290	f	ws	scrus	Scolopax rusticola	Waldschnepfe	Woodcock	Skovsneppe
333	05530	f	waw	troch	Tringa ochropus	Waldwasserläufer	Green Sandpiper	Svaleklire
334	03200	f	wf	faper	Falco peregrinus	Wanderfalke	Peregrine Falcon	Vandrefalk
335	12950	f	wal	phbor	Phylloscopus borealis	Wanderlaubsänger	Arctic Warbler	Nordsanger
336	10500	f	waa	cicin	Cinclus cinclus	Wasseramsel	Dipper	Vandstær
337	04070	f	wr	raaqu	Rallus aquaticus	Wasserralle	Water Rail	Vandrikse
338	05659	f	li	unwad	Limicolae indet.	Watvogel unbestimmt	Wader sp.	uidenficeret vadefugl
339	14420	f	wem	parmo	Parus montanus	Weidenmeise	Willow Tit	Fyrremejse
340	02649	f	wu	cispe	Circus indet.	Weihe unbestimmt	Harrier sp.	Kærhøg sp
341	06280	f	wfs	chleu	Chlidonias leucopterus	Weißflügel-Seeschwalbe	White-winged Tern	Hvidvinget Terne
342	05927	f	wm		Larus cachinnans	Weißkopfmöwe	Yellow-legged Gull	cachinnans  Måge
343	02260	f	wke	oxleu	Oxyura leucocephala	Weißkopf-Ruderente	White-headed Duck	Hvidhovedet And
344	08840	f	wrs	deleu	Picoides leucotos	Weißrückenspecht	White-backed Woodpecker	Hvidrygget spætte
345	11062	f	blkw	lusvc	Luscinia svecica cyanecula	Weißsterniges Blaukehlchen	White-spotted Bluethroat	Blåhals (cyanecula)
346	01340	f	wst	cicic	Ciconia ciconia	Weißstorch	White Stork	Hvid Stork
347	00550	f	wl	ocleu	Oceanodroma leucorhoa	Wellenläufer	Leach’s Storm-Petrel	Stor Stormsvale
348	08480	f	wh	jytor	Jynx torquilla	Wendehals	Wryneck	Vendehals
349	02310	f	wsb	peapi	Pernis apivorus	Wespenbussard	Honey-Buzzard	Hvepsevåge
350	08460	f	wi	uoepo	Upupa epops	Wiedehopf	Hoopoe	Hærfugl
351	10110	t	w	anpra	Anthus pratensis	Wiesenpieper	Meadow Pipit	Engpiber
352	02630	f	ww	cipyg	Circus pygargus	Wiesenweihe	Montagu’s Harrier	Hedehøg
353	13140	f	wg	rereg	Regulus regulus	Wintergoldhähnchen	Goldcrest	Fuglekonge
354	00462	f	yss	pupuy	Puffinus puffinus yelkouan	Yelkouan-Schwarzschnabel-Sturmtaucher	Levantine Shearwater	
355	10660	f	z	trtro	Troglodytes troglodytes	Zaunkönig	Winter Wren	Gærdesmutte
356	16649	f	zu	carsp	Carduelis indet.	Zeisig unbestimmt	Goldfinch sp.	Carduelis sp
357	07780	f	zim	caeur	Caprimulgus europaeus	Ziegenmelker	Nightjar	Natravn
358	13110	f	zi	phcol	Phylloscopus collybita	Zilpzalp	Chiffchaff	Gransanger
359	01558	f	zss	cycoc	Cygnus columbianus/cygnus	Zwerg-/Singschwan	Bewick’s Swan/Whooper Swan	Pibesvane/Sangsvane
360	01600	f	zg	anery	Anser erythropus	Zwerggans	Lesser White-fronted Goose	Dværggås
361	05780	t	zm	lamin	Larus minutus	Zwergmöwe	Little Gull	Dværgmåge
362	02200	f	zsä	mealb	Mergus albellus	Zwergsäger	Smew (merganser)	Lille Skallesluger
363	13430	f	zws	fipar	Ficedula parva	Zwergschnäpper	Red-breasted Flycatcher	Lille fluesnapper
364	05180	f	ze	lymin	Lymnocryptes minimus	Zwergschnepfe	Jack Snipe	Enkeltbekkasin
365	01532	f	zw	cycol	Cygnus columbianus bewickii	Zwergschwan (Unterart bewickii)	Bewick’s Swan	Pibesvane (bewickii)
366	06240	f	zs	stalb	Sterna albifrons	Zwergseeschwalbe	Little Tern	Dværgterne
367	05010	f	zst	camin	Calidris minuta	Zwergstrandläufer	Little Stint	Dværgryle
368	04110	f	zsu	popus	Porzana pusilla	Zwergsumpfhuhn	Baillon’s Crake	Dværgrørvagtel
369	00070	f	zt	taruf	Tachybaptus ruficollis	Zwergtaucher	Little Grebe	Lille Lappedykker
\.


--
-- Name: taxa_pkey; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_pkey PRIMARY KEY (tx_id);


--
-- Name: taxa_tx_abv_de_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_abv_de_key UNIQUE (tx_abv_de);


--
-- Name: taxa_tx_abv_dk_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_abv_dk_key UNIQUE (tx_abv_dk);


--
-- Name: taxa_tx_euring_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_euring_key UNIQUE (tx_euring);


--
-- Name: taxa_tx_name_de_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_name_de_key UNIQUE (tx_name_de);


--
-- Name: taxa_tx_name_dk_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_name_dk_key UNIQUE (tx_name_dk);


--
-- Name: taxa_tx_name_en_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_name_en_key UNIQUE (tx_name_en);


--
-- Name: taxa_tx_name_lat_key; Type: CONSTRAINT; Schema: public; Owner: huckfinn; Tablespace: 
--

ALTER TABLE ONLY taxa
    ADD CONSTRAINT taxa_tx_name_lat_key UNIQUE (tx_name_lat);


--
-- PostgreSQL database dump complete
--

