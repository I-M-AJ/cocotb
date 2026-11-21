import argparse
from pathlib import Path

from cocotb_tools.runner import get_runner


def main():
    parser = argparse.ArgumentParser(description="Run the counter cocotb test")
    parser.add_argument("--sim", choices=("vcs", "icarus"), default="vcs")
    parser.add_argument(
        "--pli-dir",
        type=Path,
        help="Verdi VCS PLI directory containing novas.tab and pli.a; enables FSDB",
    )
    args = parser.parse_args()

    build_args = []
    defines = {}
    if args.pli_dir is not None:
        if args.sim != "vcs":
            parser.error("--pli-dir requires --sim vcs")
        pli_dir = args.pli_dir.expanduser().resolve()
        table, library = pli_dir / "novas.tab", pli_dir / "pli.a"
        if not table.is_file() or not library.is_file():
            parser.error(f"{pli_dir} must contain novas.tab and pli.a")
        build_args = ["-kdb", "-P", str(table), str(library)]
        defines = {"DUMP_FSDB": 1}

    project_dir = Path(__file__).resolve().parent
    build_dir = project_dir / "sim_build" / args.sim
    runner = get_runner(args.sim)
    runner.build(
        sources=[project_dir / "counter.sv"],
        hdl_toplevel="counter",
        build_dir=build_dir,
        build_args=build_args,
        defines=defines,
        always=True,
    )
    runner.test(
        hdl_toplevel="counter",
        test_module="test_counter",
        build_dir=build_dir,
    )


if __name__ == "__main__":
    main()
