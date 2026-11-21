from pathlib import Path

from cocotb_tools.runner import get_runner


def main():
    project_dir = Path(__file__).resolve().parent
    runner = get_runner("questa")
    runner.build(
        sources=[project_dir / "counter.sv"],
        hdl_toplevel="counter",
        build_dir=project_dir / "sim_build",
        always=True,
    )
    runner.test(
        hdl_toplevel="counter",
        test_module="test_counter",
        build_dir=project_dir / "sim_build",
    )


if __name__ == "__main__":
    main()
