CREATE TABLE public.notes (
    note text
);

CREATE TABLE public.stuff (
    data text
);

COPY public.stuff (data) FROM stdin;
random data
this is not anything good
don't go here
what's up with you
\.

--
-- Data for Name: notes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.notes (note) FROM stdin;
hello
this is a note!
Lets note something important
bob has a port 25 SSH account
The password is B0bBoY25876
\.
