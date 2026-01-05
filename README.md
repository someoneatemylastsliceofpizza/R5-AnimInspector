# R5-AnimInspector
A Source .qc generator from Apex legends assets file(.rseq, .rrig and .rmdl)

---

### Usage

```bash
R5-AnimInspector.exe <parent directory> [-i <in assets season>]
```

>**Supported seasons**\
>Season 3, 9-27

>**Folder Structure**\
>├── model.rrig         # Rig file (Optional) (must be in parent directory)\
>├── model.rmdl         # Model file (Optional) (must be in parent directory)\
>└── path/
>> └── to/
>>> └── rseq/
>>>> ├── *.rseq  # Animation sequences (can be anywhere)\
>>>> ├── ...
